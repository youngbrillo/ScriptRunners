
function onSceneStart()
	genMap();

	description_text = "Course 1: door[-] | lift[ ] | pulley [ ] | trap floor[ ] | box sensor[ ]"
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
	genCamFollowers();
end

function genMap()
	-- body
	map = Scene.CreateTilemapNode("map");
	map:LoadData("./Assets/Textures/tileset.json");
	genObjects();

	App.GetCamera().zoom = 15.0;
end
function genObjects()
	objects = {
          {name="blue door", x = 12.5, y = -2.5, w=1, h=5, color = 0x6ca3c3ff, prismaticDef = {x = 0, y = 1, motorSpeed = -1.0, maxMotorForce = 10000.0, enableMotor = false, enableLimit = true, lowerTranslation = -3, upperTranslation = 0}}
        , {name="red door", x = 28.5, y = -13.5, w=1, h=3, color = 0xc36c6cff, prismaticDef = {x = 0, y = 1, motorSpeed = -1.0, maxMotorForce = 10000.0, enableMotor = false, enableLimit = true, lowerTranslation = -3, upperTranslation = 0}}
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
		end

		v.node = e;
	end
end


function genCamFollowers()
	--add camera controller to floor
	camControllers = 
	{
		 --{pos = {x = 0, y = -25}, size = {x=100, y=50}, onEnter = 20, onExit = 20, name  = "Camera Controller"}
		 {pos = {x = 18, y = -9}, size = {x=30, y=18}, onEnter = 20, onExit = 20, name  = "Camera Controller"}
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

