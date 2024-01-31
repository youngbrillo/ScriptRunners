
function onSceneStart()
	createHub();
end

function onSceneEnd() 

end
function Update(dt) 

end

function Draw() 
end

function UIDraw() 
	Raylib.DrawTextEx("arial", "Mission Giving Course", 25, Raylib.GetScreenHeight()-40, 24,1.0, 0xffffffff)
end

function onKeyPress(key)
	listenForPortalActivation(key)
end

function onBeginContact(A, B)

end


function onEndContact(A, B)

end

--/ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

function createHub()
	TextureManager.Add("Assets/Textures/dummy", "dummy");
	TextureManager.Add("Assets/Textures/kenny/inputs", "inputs")
	-- // create ground
		ground = Scene.CreateNode2d("ground");
		ground.transform.position:set(0, 1);
		ground.transform.size:set(100, 0.1);
		ground.transform:Center();
		ground.material:SetColor(0x00ff00ff)
		ground.rigidbody.bdyDef.type = 0
		ground.rigidbody.fixDef.density = 1.0;
		ground.rigidbody:SetBody(Scene.GetWorld(), ground.transform, 0)

	--// create player

	mPlayer = Scene.CreatePlayerController("Player Controller", "dummy");
	mPlayer.textureScale:set(4, 2)
	mPlayer:setPosition(-27, 0)


	--//create entrance portal and exit portal
	genEntranceAndExit()

	--//create camera controllers
	genCameraControllers();

	--//set default zoom
	App.GetCamera().zoom = 35.0;
end

function genEntranceAndExit()
	local icon ={texture="inputs", frame = {x=323, y= 170, w= 16,h= 16}}
	Portals = {
		{Name = "entrance portal", x = -30, y = -4, w = 3, h = 9 }
		,{Name = "exit portal", x = 30, y = -4, w = 3, h = 9 , destination = "obstacleCourse"}
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
			e:setObserver(mPlayer);

			local function TransitionToScene()
				App.GoToScene(v.destination or "MissionGivingCourse")
				
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


function genCameraControllers( )

	camControllers = 
	{
		   {pos = {x = 0, y = -2}, size = {x=80, y=6}, onEnter = 45, onExit = 20, name  = "Street Cam"}
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
		e:SetTarget(mPlayer);
		v.node = e;

	end
end

