
function onSceneStart()
	genMap();

	description_text = "Course 1: door [-] | lift[-] | pulley [-] | switches [ ] | NPCs [ ] | trap floor[ ]"
end

function onSceneEnd() 

end
function Update(dt) 

end

function Draw() 
end

function UIDraw() 
	
	Raylib.DrawTextEx("arial", description_text, 25, Raylib.GetScreenHeight()-40, 24,1.0, 0xffffffff)
end

spawn_player_key = 81; --//Q
function onKeyPress(key)
	if key == spawn_player_key then
		SpawnPlayer();
	end

	listenForSwitchStateChange(key);
end

function onBeginContact(A, B)
	handleSwitchTouch(A, B, true);
end


function onEndContact(A, B)
	handleSwitchTouch(A, B, false);

end

function SpawnPlayer()

	if(mPlayer ~= nil) then
		mPlayer.alive = false;
		RemoveCameraFollowers();
	end

	TextureManager.Add("Assets/Textures/dummy", "dummy")

	mPlayer = Scene.CreatePlayerController("Player Controller", "dummy");
	mPlayer.textureScale:set(4, 2)
	mPlayer:setPosition(0, -10)
	--construct new camera's to observe the 'new' player
	genCamFollowers();
	--attache switches to the 'new' player
	for k, v in ipairs(Switches) do
		v.node:setObserver(mPlayer);
	end
end

function genMap()
	-- body
	map = Scene.CreateTilemapNode("map");
	map:LoadData("./Assets/Textures/tileset.json");
	genObjects();
	App.GetCamera().zoom = 7.0;
end
function genObjects()
	Switches ={};
	objects = {
          {name="blue door", x = 12.5, y = -2.5, w=1, h=5, color = 0x6ca3c3ff, prismaticDef = {x = 0, y = 1, motorSpeed = -1.0, maxMotorForce = 10000.0, enableMotor = false, enableLimit = true, lowerTranslation = -3, upperTranslation = 0}}
        , {name="red door", x = 28.5, y = -13.5, w=1, h=3, color = 0xc36c6cff, prismaticDef = {x = 0, y = 1, motorSpeed = -1.0, maxMotorForce = 10000.0, enableMotor = false, enableLimit = true, lowerTranslation = -3, upperTranslation = 0}}
        , {	name="elevator", x = -14.5, y = -25, w=4.75, h=1, color = 0x6c6ca3ff, 
			prismaticDef = {x = 0, y = 1, motorSpeed = -7.0, maxMotorForce = 10000.0, enableMotor = false, enableLimit = true, lowerTranslation = -25, upperTranslation = 3},
			switch = {x = -26, y = -26, w = 1, h= 2}
		}
        , {
			name="motor", x = -14.5, y = -6, w=1, h=1, color = 0x6c6ca3ff, 
			prismaticDef = {x = 0, y = 1, motorSpeed = -5.0, maxMotorForce = 10000.0, enableMotor = false, enableLimit = true, lowerTranslation = -5, upperTranslation = 0},
			switch = {x = -11, y = -1, w = 1, h= 2}
		}
	}

	for k, v in ipairs(objects) do
		local e = Scene.CreateNode2d(v.name);
		e.transform.position:set(v.x, v.y);
		e.transform.size:set(v.w, v.h);
		e.transform:Center();
		e.material:SetColor(v.color);
		e.rigidbody.bdyDef.type = 2
		e.rigidbody.fixDef.density = 1.0;
		e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)

		if(v.prismaticDef) then
			v.joint = b2d.CreatePrismaticJoint(Scene.GetWorld(), map, e, v.prismaticDef)
			if v.switch then
				genSwitch(e,v.joint, v.prismaticDef, v.switch )
			end
		end

		v.node = e;
	end

	--[[
	]]
	genPully(
		{name="left lift", x = -32.5, y = -18, w = 4.5, h =1}, 
		{name="right lift", x = -14.5, y = -10, w = 4.5, h =1},
		{type = 2, fixed = true, density = 10.0, AnchorA = {x = 0, y = -9}, AnchorB = {x = 0, y = -17}, color={r = 234/255, g=16/255, b=98/255,a= 0.7}}
	);

end
function genSwitch(c_object, joint, joint_settings, config)
	TextureManager.Add("Assets/Textures/kenny/inputs", "inputs")

	local mSwitch = {};
	local icon ={texture="inputs", frame = {x=323, y= 170, w= 16,h= 16}}
	local e = Scene.CreateInteractableNode(c_object.Name.."_on_off_switch");
		e.transform.position:set(config.x, config.y);
		e.transform.size:set(config.w, config.h)
		e.transform:Center();
		e.rigidbody.bdyDef.type = 1;
		e.rigidbody.fixDef.isSensor = true;
		e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)
		e:setIconDestination(0, -(e.transform.size.y + 0.5), 1.0, 1.0)
		e:setIconTexture(icon.texture)
		e:setIconFrame(icon.frame.x, icon.frame.y, icon.frame.w, icon.frame.h)
		--e:setObserver(mPlayer);


	local function toggleMotor()
		if not joint:IsMotorEnabled() then
			joint:EnableMotor(true);
			print("Motor: ".. c_object.Name.." Activated! ("..joint_settings.motorSpeed.." N)");
		else
			joint:EnableMotor(false);
			joint_settings.motorSpeed = -1 * joint_settings.motorSpeed;
			joint:SetMotorSpeed(joint_settings.motorSpeed);
			print("Motor: ".. c_object.Name.." Deactivated!");
		end
	end
	
	mSwitch.node = e;
	mSwitch.Activation_key = 69; -- KEY_E
	mSwitch.onBeginContact = toggleMotor;
	table.insert(Switches, mSwitch)
end

function handleSwitchTouch(A, B, BeginingContact)
	local other = nil;
	local target = nil
	local object = nil;
	for k, v in ipairs(Switches) do
		if(v.node.Name == A.Name) then
			target = v;
			other = B;
			object = v;
			break;
		elseif v.node.Name == B.Name then
			target = v;
			other = A;
			object = v;
			break;
		end
	end

	if other ~= nil and target ~= nil then
		if other.Name == target.node.observer.Name then
			object.isActivated = BeginingContact;
		end
	end
end

function listenForSwitchStateChange(key)
	for k, v in ipairs(Switches) do
		if v.isActivated and key == v.Activation_key and v.onBeginContact ~= nil then
			v.onBeginContact();
		end
	end
end



function genPully(A, B, config)
	local a = Scene.CreateCameraController2d(A.name);
		a.transform.position:set(A.x, A.y);
		a.transform.size:set(A.w, A.h)
		a.transform:Center();
		a.rigidbody.bdyDef.type = config.type;
		a.rigidbody.bdyDef.fixedRotation = config.fixed;
		a.rigidbody.fixDef.density = config.density;
		a.rigidbody:SetBody(Scene.GetWorld(), a.transform, 0)
		a.material:SetColorVec(config.color.r,config.color.g,config.color.b,config.color.a)

		
	local b = Scene.CreateNode2d(B.name);
		b.transform.position:set(B.x, B.y);
		b.transform.size:set(B.w, B.h)
		b.transform:Center();
		b.rigidbody.bdyDef.type = config.type;
		b.rigidbody.bdyDef.fixedRotation = config.fixed;
		b.rigidbody.fixDef.density = config.density;
		b.rigidbody:SetBody(Scene.GetWorld(), b.transform, 0)
		b.material:SetColorVec(config.color.r,config.color.g,config.color.b,config.color.a)

		b2d.CreatePullyJoint( Scene.GetWorld(), a, b, 
				{
					x1 = A.x ,			--+ A.w/2, 
					y1 = A.y - A.h/2, 
					x2 = B.x ,			--+ B.w/2, 
					y2 = B.y - B.h/2,	--config.AnchorB.y, 
					x3 = A.x + config.AnchorA.x, 
					y3 = A.y + config.AnchorA.y, 
					x4 = B.x + config.AnchorB.x, 
					y4 = B.y + config.AnchorB.y, 
					ratio = 1.0
				} 
			);
end


function genCamFollowers()
	--add camera controller to floor
	camControllers = 
	{
		   {pos = {x = 18, y = -9}, size = {x=30, y=18}, onEnter = 20, onExit = 20, name  = "Door Cam"}
		 , {pos = {x = -32, y = -16}, size = {x=38, y=31}, onEnter = 20, onExit = 20, name  = "warehouse Cam"}
	}

	for k, v in ipairs(camControllers) do
		local e  = Scene.CreateCameraController2d(v.name);
		e.transform.position:set(v.pos.x, v.pos.y);
		e.transform.size:set(v.size.x, v.size.y);
		e.transform:Center();
		e.material:SetColorVec(1,1,1,0.5)
		e:SetTarget(mPlayer);
		e:Standardize();
		e.onEnter.zoom = v.onEnter;
		e.onExit.zoom = v.onExit;
		e.visible = false;
		v.node = e;
	end
end

function RemoveCameraFollowers()
	for k, v in ipairs(camControllers) do
		v.node.alive = false;

	end

end
