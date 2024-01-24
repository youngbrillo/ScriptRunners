function onSceneStart()
	GenTileMap();
end

function onSceneEnd() 

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
	map = Scene.CreateTilemapNode("tile map")
	--map:LoadData("./Assets/Textures/stringstar_fields/tileset.json");
	map:Import('./Assets/Textures/ruins/tiles.png', './Assets/Textures/ruins/ruins.json')
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
