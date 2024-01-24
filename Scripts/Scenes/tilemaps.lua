function onSceneStart()
	GenTileMap();
end

function onSceneEnd() 
	RemoveBackGroundTextures();
end
function Update(dt) 

end

function Draw() 
	--drawGrid(-10, 10, 1);
end

function UIDraw() 
end

spawn_player_key = 81; --//Q

function onKeyPress(key)
	--print(key)
	if key == spawn_player_key then
		SpawnPlayer();
	end
end





function drawGrid(min, max, spacing)
	for y = min, max, spacing do
		for x = min, max, spacing do
			Raylib.DrawLine(x, min, x, max, 0xffffff7e)
		end
		Raylib.DrawLine(min, y, max, y, 0xffffff7e)
	end
end

function GenTileMap() 
	local genruins = false;
	if genruins then
		map = Scene.CreateTilemapNode("tile map")
		map:Import('./Assets/Textures/ruins/tiles.png', './Assets/Textures/ruins/ruins.json')
	else
		LoadBackgroundTextures(60, 30);
		map = Scene.CreateTilemapNode("tile map")
		map.transform.position.x = 8;
		map:Import('./Assets/Textures/stringstar_fields/tileset.png', "./Assets/Textures/stringstar_fields/starfields.json")
	end

	App.GetCamera().zoom = 50.0;
end


function SpawnPlayer()

	if(mPlayer ~= nil) then
		mPlayer.alive = false;
	end

	TextureManager.Add("Assets/Textures/dummy", "dummy")
	mPlayer = Scene.CreatePlayerController("Player Controller", "dummy");
	mPlayer.textureScale:set(4, 2)
	mPlayer:setPosition(0, -10)
end

function LoadBackgroundTextures(width, height)
	bgTextures = {};
	local base_path = "Assets/Textures/";
	local cloud_path = "clouds_craft-pixel/Clouds/Clouds 3/"
	table.insert(bgTextures, {id = TextureManager.Add(base_path..cloud_path.."1", "bg_1"), name = "bg_1"});
	table.insert(bgTextures, {id = TextureManager.Add(base_path..cloud_path.."2", "bg_2"), name = "bg_2"});
	table.insert(bgTextures, {id = TextureManager.Add(base_path..cloud_path.."3", "bg_3"), name = "bg_3"});
	table.insert(bgTextures, {id = TextureManager.Add(base_path..cloud_path.."4", "bg_4"), name = "bg_4"});
	
	--table.insert(bgTextures, {id = TextureManager.Add(base_path..cloud_path.."5", "bg_5"), name = "bg_5"});
	--table.insert(bgTextures, {id = TextureManager.Add(base_path..cloud_path.."6", "bg_6"), name = "bg_6"});
	

	for k, v in ipairs(bgTextures)
	do
		local p = Scene.CreateNode2d("Background-"..v.name);
		p.transform.size.x = width;
		p.transform.size.y = height;
		p.transform:Center();
		p.material:SetTextureByAlias(v.name)
		p.material.uv_scroll:set(2 * (k-1), 0)

	end



	
	backgrounds_initiated = true;
end

function RemoveBackGroundTextures()
	if backgrounds_initiated then
		for k, v in ipairs(bgTextures)
		do
			TextureManager.Remove(v.id);
		end
	end
end