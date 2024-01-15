
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
	local folder_path = "clouds_craft-pixel/city-backgrounds/city 6/"

	table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."1", "bg_1"), name = "bg_1"});
	table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."2", "bg_2"), name = "bg_2"});
	table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."3", "bg_3"), name = "bg_3"});
	table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."4", "bg_4"), name = "bg_4"});
	
	table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."5", "bg_5"), name = "bg_5"});
	table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."6", "bg_6"), name = "bg_6"});
	
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
         {name="floor", x = 00,	y = 20,	w=100,  h=.5 , blocking =true, color = 0x6ca3c3ff , hasShadow = false, dynamic = false}
	}

	for k, v in ipairs(elements) do
		local e = Scene.CreateNode2d(v.name);
		e.transform.position:set(v.x, v.y);
		e.transform.size:set(v.w, v.h);
		e.transform:Center();
		e.material:SetColor(v.color);
		if(v.dynamic) then
			e.rigidbody.bdyDef.type = 2
			e.rigidbody.fixDef.density = 1.0;
		end
		e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)
	end
	App.GetCamera().zoom = 18;
end
function CreateObjects() 
	TextureManager.Add("Assets/Textures/dummy", "dummy")
	mPlayer = Scene.CreatePlayerController("Player Controller", "dummy");
	mPlayer.textureScale:set(4, 2)

	
	--add camera controller to floor
	local camControllers = 
	{
		{pos = {x = 0, y = 17.25}, size = {x=85, y=5}, onEnter = 34, onExit = 18, name  = "Floor Cam Controller"},
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
		  {pos = {x = -50+2.5,  y = 17.25}, size = {x = 5, y = 5}, name="Exit to Platformer II", destination = "Platforming II"}
		, {pos = {x = 50-2.5,  y = 17.25}, size = {x = 5, y = 5}, name="Exit to Platformer I", destination = "Platforming"}
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
end
function DrawInstructions() 
	Raylib.DrawText("Platformer III", 25, 25, 25, 0xffffffff)
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