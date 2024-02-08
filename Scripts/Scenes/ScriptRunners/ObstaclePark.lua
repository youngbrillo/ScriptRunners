function onSceneStart()
	TextureManager.Add("Assets/Textures/dummy", "dummy")
	genMap();

	description_text = "Course 2: movement [ ] | everything else [ ]"
end

function onSceneEnd() 

end
function Update(dt) 

end
gsp = 5; gspOff = 50;
function Draw() 
	--drawGrid(gsp, gsp + gspOff, 1);
end

function UIDraw() 
	Raylib.DrawTextEx("arial", description_text, 25, Raylib.GetScreenHeight()-40, 24,1.0, 0xffffffff)
end

spawn_player_key = 81; --//Q
function onKeyPress(key)
	if key == spawn_player_key then
		SpawnPlayer(0, -10);
	end
end

function onBeginContact(A, B)

end
function onEndContact(A, B)

end

--// -----------------------------------------------------// -----------------------------------------------------//
function drawGrid(min, max, spacing)
	for y = min, max, spacing do
		for x = min, max, spacing do
			Raylib.DrawLine(x, min, x, max, 0xffffff7e)
		end
		Raylib.DrawLine(min, y, max, y, 0xffffff7e)
	end
end
function SpawnPlayer(x, y )
	if(mPlayer ~= nil) then
		mPlayer.alive = false;
	end

	mPlayer = Scene.CreatePlayerController("Player Controller", "dummy");
	--mPlayer = Scene.CreatePlayerController2("Player Controller", "dummy", "Player.ini");
	mPlayer.textureScale:set(4, 2)
	mPlayer:setPosition(x,y)
end


function genMap()
	-- body
	map = Scene.CreateTilemapNode("map");
	map:LoadData("./Assets/Textures/obstaclePark.json");
	App.GetCamera().zoom = 7.0;
end