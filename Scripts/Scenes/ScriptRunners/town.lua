
function onSceneStart()
	TextureManager.Add("Assets/Textures/dummy", "dummy");
	TextureManager.Add("Assets/Textures/kenny/inputs", "inputs")
	CreateBackGrounds();
	createTown();
	App.GetCamera().zoom = 25.0;
end

function onSceneEnd() 
	RemoveBackGroundTextures();
end
function Update(dt) 

end

function Draw() 

end
function UIDraw() 
	Raylib.DrawTextEx("arial", "Town Hub", 25, Raylib.GetScreenHeight()-60, 40 ,1.0, 0xffffffff)
end

spawn_player_key = 81; --//Q
function onKeyPress(key)
	if key == spawn_player_key then
		SpawnPlayer(-40, 12);
	end
end
function onBeginContact(A, B)

end
function onEndContact(A, B)

end
mPlayer = nil
npcs = {};
function SpawnPlayer(x, y )
	if(mPlayer ~= nil) then
		mPlayer.alive = false;
	end

	--mPlayer = Scene.CreatePlayerController("Player Controller", "dummy");
	mPlayer = Scene.CreatePlayerController2("Player Controller", "dummy", "Player.ini");
	mPlayer.textureScale:set(4, 2)
	mPlayer:setPosition(x,y)

	for k, v in ipairs(npcs) do
		v.node.prompter = mPlayer;
	end
end

--/ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function CreateBackGrounds() 
	bgTextures = {};
	local base_path = "Assets/Textures/";
	local folder_path = "clouds_craft-pixel/Clouds/Clouds 1/"
		table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."1", "bg_1"), name = "bg_1", uv_x = 2});
		table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."2", "bg_2"), name = "bg_2", uv_x = 4});
		table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."3", "bg_3"), name = "bg_3", uv_x = 6});
		table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."4", "bg_4"), name = "bg_4", uv_x = 8});

	
	local cityNumber = "01"
	folder_path = "DigitalMoons/Pixelcity"..cityNumber.."/"
		--table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."Pixelcity"..cityNumber.."_layer" .. "01", "cityLayer1"), name = "cityLayer1", uv_x = 2});
		--table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."Pixelcity"..cityNumber.."_layer" .. "02", "cityLayer2"), name = "cityLayer2", uv_x = 4});
		--table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."Pixelcity"..cityNumber.."_layer" .. "03", "cityLayer3"), name = "cityLayer3", uv_x = 5});
		--table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."Pixelcity"..cityNumber.."_layer" .. "04", "cityLayer4"), name = "cityLayer4", uv_x = 6});
		--table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."Pixelcity"..cityNumber.."_layer" .. "05", "cityLayer5"), name = "cityLayer5", uv_x = 0});
		--table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."Pixelcity"..cityNumber.."_layer" .. "06", "cityLayer6"), name = "cityLayer6", uv_x = 8});

	table.insert(bgTextures, {id = TextureManager.Add(base_path.."city/street", "m_city_street"), name = "m_city_street", uv_x = 0, x = 0, y = -4.5, w = 100, h = 50});

	for k, v in ipairs(bgTextures)
	do
		local p = Scene.CreateNode2d("Background-"..v.name);
		p.transform.position.y = v.y or 0
		p.transform.size.x = v.w or 100;
		p.transform.size.y = v.h or 40;
		p.transform:Center();
		p.material:SetTextureByAlias(v.name)
		p.material.uv_scroll:set(v.uv_x, 0)
	end
end

function RemoveBackGroundTextures()
	for k, v in ipairs(bgTextures)
	do
		TextureManager.Remove(v.id);
	end
end
--/ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


function createTown()
	map = Scene.CreateTilemapNode("tile map")
	map:LoadData("./Assets/Textures/city/Town.json");
	SpawnPlayer(-40, 12);

	--//generate characters
		local icon = {alias = "inputs", frame = {x=323, y= 170, w= 16,h= 16}};

	npcs = {
		{name = "man a", text = "H-Hello, I can tell you what to do...\nI-I mean I can give you assignements!", font = "comic", x = -37, y = 12},
		{name = "man b", text = "Ahem! If you need directions look no further!", font = "comic", x = -18, y = 12},
		{name = "man c", text = "O-Ohhhh! I believe I need some mail. \nDo you have any mail?", font = "comic", x = -9, y = 12},
	}
	-- Gen Task Master
	for k, v in ipairs(npcs) do
		local e = Scene.CreateNPCNode(v.name, "dummy", icon.alias, "npc.script.lua");
			e.transform.position:set(v.x, v.y);
			e.transform.size:set(0.5, 1);
			e.transform:Center();
			e.textureScale:set(4, 2)
			e.rigidbody.bdyDef.type = 2;
			e.rigidbody.bdyDef.fixedRotation = true;
			e.rigidbody.fixDef.density = 1.0;
			e.rigidbody.fixDef.friction = 1.0;
			e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)
			e.icon.frame:set(icon.frame.x,icon.frame.y,icon.frame.w, icon.frame.h)
			e.text:setText(v.text)
			e.text.fontSize = 32
			e.text:SetFont(v.font)
			e.prompter = mPlayer;
			print(e.Name)
			v.node = e;
			b2d.AddCircleSensor(e.rigidbody, 1.25)
	end
end
