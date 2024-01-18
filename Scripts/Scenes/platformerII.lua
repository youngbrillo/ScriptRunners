
function onSceneStart()
	CreateBackGrounds();
	CreateEnvironments();
	CreateObjects();
end

function onSceneEnd() 
	RemoveBackGroundTextures();
end
function Update(dt) 
end

function Draw() 
end

function UIDraw() 
	DrawInstructions();
end

function onBeginContact(A, B)
	ListenForSceneTransitionContact(A, B);
end

function onEndContact(A, B)

end



function CreateBackGrounds() 
	bgTextures = {};
	local base_path = "Assets/Textures/";
	local folder_path = "clouds_craft-pixel/Clouds/Clouds 4/"
	table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."1", "bg_1"), name = "bg_1"});
	table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."2", "bg_2"), name = "bg_2"});
	table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."3", "bg_3"), name = "bg_3"});
	table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."4", "bg_4"), name = "bg_4"});
	for k, v in ipairs(bgTextures)
	do
		local p = Scene.CreateNode2d("Background-"..v.name);
		p.transform.size.x = 100;
		p.transform.size.y = 40;
		p.transform:Center();
		p.material:SetTextureByAlias(v.name)
		p.material.uv_scroll:set(2 * (k-1), 0)
	end
end

function RemoveBackGroundTextures()
	for k, v in ipairs(bgTextures)
	do
		TextureManager.Remove(v.id);
	end
end
function CreateEnvironments() 

	elements =  {
         {name="floor", x = 22.5,	y = 10,	w=55,  h=.5 , blocking =true, color = 0x6ca3c3ff , hasShadow = false, dynamic = false}
         , {name="floor frag.1", x = -32.375,	y = 10,	w=35.25,  h=.5 , blocking =true, color = 0x6ca3c3ff , hasShadow = false, dynamic = false}
         , {name="sub floor 1", x = -7.25,	y = 15,	w=15,  h=.5 , blocking =true, color = 0x6ca3c3ff , hasShadow = false, dynamic = false}
         , {name="sub wall.l", x = -15,	y = 12.75,	w=.5,  h=5 , blocking =true, color = 0x6ca3c3ff , hasShadow = false, dynamic = false}
         , {name="sub wall.r", x = 0.5,	y = 12.75,	w=.5,  h=5 , blocking =true, color = 0x6ca3c3ff , hasShadow = false, dynamic = false}
         , {name="float floor 1", x = 7.75,	y = 5,	w=15,  h=.5 , blocking =true, color = 0x6ca3c3ff , hasShadow = false, dynamic = false}
         , {name="Large obstacle a", x = -11,	y = 7.225,	w=15,  h=5 , blocking =true, color = 0x6ca3c3ff , density = 10.0, dynamic = true}
         , {name="Large obstacle b", x = 12.75,	y =2.25,	w=5,  h=5 , blocking =true, color = 0x6ca3c3ff , density = 5.0, dynamic = true}
         , {name="top wall.r", x = 15.5,	y = 2.75,	w=.5,  h=5 , blocking =true, color = 0x6ca3c3ff , hasShadow = false, dynamic = false}
	}

	for k, v in ipairs(elements) do
		local e = Scene.CreateNode2d(v.name);
		e.transform.position:set(v.x, v.y);
		e.transform.size:set(v.w, v.h);
		e.transform:Center();
		e.material:SetColor(v.color);
		if(v.dynamic) then
			e.rigidbody.bdyDef.type = 2
			e.rigidbody.fixDef.density = v.density
		end
		e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)
	end
	App.GetCamera().zoom = 48;
	App.GetCamera().target:set(-3, 8.5);
end
function CreateObjects() 
	TextureManager.Add("Assets/Textures/dummy", "dummy")
	mPlayer = Scene.CreatePlayerController("Player Controller", "dummy");
	mPlayer.textureScale:set(4, 2)
	mPlayer:setPosition(-3.0, 8.5)

	--add camera controller to floor
	local camControllers = 
	{
		{pos = {x = 0, y = 7.25}, size = {x=85, y=5}, onEnter = 34, onExit = 18, name  = "Floor Cam Controller"},
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
	end
	
	
	exitListeners = 
	{
		  {pos = {x = -12.5,  y = 12.5}, size = {x = 4.5, y = 4.5}, name="Exit to Platformer I", destination = "Platforming"}
		, {pos = {x = 13,  y = 2.5}, size = {x = 4, y = 4}, name="Exit to Platformer III", destination = "Platforming III"}
	}
	
	for k, v in ipairs(exitListeners) do
		local e  = Scene.CreateNode2d(v.name);
		e.transform.position:set(v.pos.x, v.pos.y);
		e.transform.size:set(v.size.x, v.size.y);
		e.transform:Center();
		e.material:SetColorVec(0.5, 0.5, 0.5,0.5)
		e.visible = true;
		e.rigidbody.fixDef.isSensor = true;
		e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)
		v.node = e;
	end


	TextureManager.Add("Assets/Textures/buttons", "buttons")
	--//create triggers to move the boxes
	interactables = {
		{name="big box mover", pos={x = 7, y=9.25}, size={x = 2, y = 1}, texture="buttons"}
	}
	for k, v in ipairs(interactables) do
		local e = Scene.CreateInteractableNode(v.name);

		e.transform.position:set(v.pos.x, v.pos.y);
		e.transform.size:set(v.size.x, v.size.y);
		e.transform:Center();
		v.node = e;
		e.material:SetTexture(v.texture)
		e.material.source:set(0, 5, 64, 64);
		e.textureScale.y = 2;

		e.rigidbody.fixDef.isSensor = false;
		e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)
		e:setIconDestination(0, -(e.transform.size.y + 0.5), 1.0, 1.0)
		e:setObserver(mPlayer);
	end
end
function DrawInstructions() 
	Raylib.DrawText("Platformer II", 25, 25, 25, 0xffffffff)

end

function ListenForSceneTransitionContact(A, B)
	local other = nil;
	local target = nil
	for k, v in ipairs(exitListeners) do
		if(v.node.Name == A.Name) then
			target = v;
			other = B;
			break;
		elseif(v.node.Name == B.Name) then
			target = v;
			other = A;
			break;
		end
	end

	if(other ~= nil and target ~= nil) then
		if(other.Name == mPlayer.Name) then
			App.GoToScene(target.destination)
		end
	end
end