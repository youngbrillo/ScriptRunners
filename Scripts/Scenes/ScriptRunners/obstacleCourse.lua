
function onSceneStart()
	TextureManager.Add("Assets/Textures/dummy", "dummy")
	genMap();

	description_text = "Course 1: door [-] | lift [-] | pulley [-] | switches [-] | NPCs [-]"
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
	listenForPortalActivation(key);
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


	mPlayer = Scene.CreatePlayerController("Player Controller", "dummy");
	mPlayer.textureScale:set(4, 2)
	mPlayer:setPosition(0, -10)
	--construct new camera's to observe the 'new' player
	genCamFollowers();
	--attache switches to the 'new' player
	for k, v in ipairs(Switches) do
		v.node:setObserver(mPlayer);
	end

	for k, v in ipairs(NPCS) do
		v.node.prompter = mPlayer;
	end

	for k, v in ipairs(Portals) do
		v.node:setObserver(mPlayer);
	end
end

function genMap()
	-- body
	map = Scene.CreateTilemapNode("map");
	map:LoadData("./Assets/Textures/tileset.json");
	genObjects();
	genCharacters();
	genEntranceAndExit();
	App.GetCamera().zoom = 7.0;
end
function genObjects()
	Switches ={};
	objects = {
          {name="blue door", x = 12.5, y = -2.5, w=1, h=5, color = 0x6ca3c3ff, prismaticDef = {x = 0, y = 1, motorSpeed = -1.0, maxMotorForce = 10000.0, enableMotor = false, enableLimit = true, lowerTranslation = -3, upperTranslation = 0},
			switch = {x = 32.5, y = -13, w = 1, h = 2}, switchContactFunc = ToggleMotorAndRecordState		
		  }
        , {name="red door", x = 28.5, y = -13.5, w=1, h=3, color = 0xc36c6cff, prismaticDef = {x = 0, y = 1, motorSpeed = -1.0, maxMotorForce = 10000.0, enableMotor = false, enableLimit = true, lowerTranslation = -3, upperTranslation = 0},
			switch = {x = -22.5, y = -51, w = 1, h = 2}			
		  }
        , {	name="elevator", x = -14.5, y = -25, w=4.75, h=1, color = 0x6c6ca3ff, 
			prismaticDef = {x = 0, y = 1, motorSpeed = -7.0, maxMotorForce = 10000.0, enableMotor = false, enableLimit = true, lowerTranslation = -25, upperTranslation = 3},
			switch = {x = -17.5, y = -26, w = 1, h= 2}
		}
        , {
			name="motor", x = -14.5, y = -6, w=1, h=1, color = 0x6c6ca3ff, 
			prismaticDef = {x = 0, y = 1, motorSpeed = -5.0, maxMotorForce = 10000.0, enableMotor = false, enableLimit = true, lowerTranslation = -5, upperTranslation = 0},
			switch = {x = -11, y = -1, w = 1, h= 2}
		}
        , {name="down_motor", x = -34.5, y = -29, w=1, h=2, color = 0x7c7c7cff, prismaticDef = {x = 0, y = 1, motorSpeed = -5.0, lowerTranslation = 0, upperTranslation = 26, maxMotorForce = 10000.0, enableMotor = true, enableLimit = true},
			switch = {x = -35, y = -7, w = 1, h = 2}			
		}
        , {name="up_motor", x = -30.5, y = -1, w=1, h=2, color = 0x7c7c7cff, prismaticDef = {x = 0, y = 1, motorSpeed = -5.0, lowerTranslation = -20, upperTranslation = 0, maxMotorForce = 10000.0, enableMotor = false, enableLimit = true},
			switch = {x = -35, y = -7, w = 1, h = 2}			
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
				genSwitch(e,v.joint, v.prismaticDef, v.switch , v.switchContactFunc)
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
function genSwitch(c_object, joint, joint_settings, config, beginFunction)
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


	local function switchMotorState()
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
	

	local function toggleMotor()
		if not joint:IsMotorEnabled() then
			joint:EnableMotor(true);
		end
		joint:SetMotorSpeed(joint_settings.motorSpeed);
		joint_settings.motorSpeed = -1 * joint_settings.motorSpeed;
	end

	mSwitch.node = e;
	mSwitch.Activation_key = 69; -- KEY_E
	mSwitch.onBeginContact = beginFunction or toggleMotor;
	mSwitch.joint = joint;
	mSwitch.joint_settings = joint_settings;

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
			v.onBeginContact(v);
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
		 , {pos = {x = -15, y = -43}, size = {x=5, y=22}, onEnter = 20, onExit = 20, name  = "elevator Cam"}
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

function genCharacters()
	NPCS = {}
	--SpawnPlayer()

	local icon = {alias = "inputs", frame = {x=323, y= 170, w= 16,h= 16}};
	NPCS = {
		{
			name = "Blue-door man", pos = {x= 1, y = 0}, size = {x = 0.5, y = 1}, alias = "dummy", 
			text  = {string = "Hey can you open that blue door for me?", fontSize = 32, fontAlias = "comic", contactRadius = 1.0},
			DialogueFunc = instructionset_openBlueDoor
		},
		{
			name = "Red-door Man", pos = {x = -20, y = -27}, size = {x = 0.5, y = 1}, alias = "dummy", 
			text  = {string = "Oh did blue-door man send you my way?\nWell, the red door has to be opened first.\nGo hit the switch upstairs.", fontSize = 32, fontAlias = "comic", contactRadius = 1.0},
			DialogueFunc = instructionset_openRedDoor
		},
		{
			name = "Switch-Switch Man", pos = {x= -38, y = -7}, size = {x = 0.5, y = 1}, alias = "dummy", 
			text  = {string = "You can interact with the door the same way you interact with me n my brothers.", fontSize = 32, fontAlias = "comic", contactRadius = 1.0},
			DialogueFunc = instructionset_OperateSwitches
		},
	}

	for k, v in ipairs(NPCS) do
	
		local e = Scene.CreateNPCNode(v.name, v.alias, icon.alias, "no script");

		e.transform.position:set(v.pos.x, v.pos.y);
		e.transform.size:set(v.size.x, v.size.y);
		e.transform:Center();
		e.textureScale:set(4, 2)
		e.rigidbody.bdyDef.type = 2;
		e.rigidbody.bdyDef.fixedRotation = true;
		e.rigidbody.fixDef.density = 1.0;
		e.rigidbody.fixDef.friction = 1.0;
		e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)
		e.icon.frame:set(icon.frame.x,icon.frame.y,icon.frame.w, icon.frame.h)
		e.text:setText(v.text.string, true)
		e.text.fontSize = v.text.fontSize
		e.text:SetFont(v.text.fontAlias)
		b2d.AddCircleSensor(e.rigidbody, v.text.contactRadius)
		v.node = e;
		v.d_index = 1;
	end
end

function onDialogueStart(character, player )
	for k,v in ipairs(NPCS) do
		if character:GetID() == v.node:GetID() then
			v.DialogueFunc(v, true);
		end
	end
end


function onDialogueEnd(character, player )
	for k,v in ipairs(NPCS) do
		if character:GetID() == v.node:GetID() then
			v.DialogueFunc(v, false);
		end
	end
end

function instructionset_openBlueDoor(NPC, isBegin) 
	local dialogue = {
		"You might need to talk to my brother over there to learn more.",
		"He can teach you how to operate a switch. It's real easy."
	}

	if blue_door_up then
		
		dialogue = {"Hey Thanks!\nNow we can procceed to the next zone!", "I wonder what's out there..."}
	end

	if( not isBegin) then
		NPC.node.text:setText(dialogue[NPC.d_index], true);
		NPC.d_index = NPC.d_index + 1;
		if(NPC.d_index > #dialogue) then
			NPC.d_index	= 1;
		end
	end
end
function instructionset_openRedDoor(NPC, isBegin) 

	local dialogue = {
		"It'll open your path and then you can be on your way.",
		"What'da'ya want kid?"
	}
	if( not isBegin) then
		NPC.node.text:setText(dialogue[NPC.d_index], true);
		if(NPC.d_index < #dialogue) then
			NPC.d_index = NPC.d_index + 1;
		end
	end
end
function instructionset_OperateSwitches(NPC, isBegin) 
	local dialogue = {
		"Just press the interact key.\nI mean, you already did it to speak to me.",
		"Try to make that platform fall down, and take it back up to my other brother.\n He'll tell you more."
	}
	if( not isBegin) then
		NPC.node.text:setText(dialogue[NPC.d_index], true);
		NPC.d_index = NPC.d_index + 1;
		if(NPC.d_index > #dialogue) then
			NPC.d_index	= 1;
		end
	end
end

blue_door_up = false;
function ToggleMotorAndRecordState(switch)

		if not switch.joint:IsMotorEnabled() then
			switch.joint:EnableMotor(true);
		end
		switch.joint:SetMotorSpeed(switch.joint_settings.motorSpeed);
		switch.joint_settings.motorSpeed = -1 * switch.joint_settings.motorSpeed;
		blue_door_up = not blue_door_up;
		print("Blue door is?", blue_door_up)
end

function genEntranceAndExit()
	local icon ={texture="inputs", frame = {x=323, y= 170, w= 16,h= 16}}
	Portals = {
		{Name = "exit portal", x = 28, y = -4.5, w = 10, h = 9 , destination = "MissionGivingCourse"}
	}

	for k, v in ipairs(Portals) do
	
		local e = Scene.CreateInteractableNode(v.Name);
			e.transform.position:set(v.x, v.y);
			e.transform.size:set(v.w, v.h)
			e.transform:Center();
			e.rigidbody.bdyDef.type = 1;
			e.rigidbody.fixDef.isSensor = true;
			e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)
			e:setIconDestination(0, 0, 2.0, 2.0)
			e:setIconTexture(icon.texture)
			e:setIconFrame(icon.frame.x, icon.frame.y, icon.frame.w, icon.frame.h)
			e.visible = false;

			local function TransitionToScene()
				App.GoToScene(v.destination or "obstacleCourse")
				
			end
			v.node = e;
			v.Activation_key = 69; -- KEY_E
			v.onBeginContact = TransitionToScene;

	end
end

function listenForPortalActivation(key)
	for k, v in ipairs(Portals) do
		if v.node.isInteractive and key == v.Activation_key then
			v.onBeginContact(v);
		end
	end
end
