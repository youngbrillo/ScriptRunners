
--Player = require("Scripts/Scenes/Player")
Player = {}
LIGHTGRAY = 0xC8C8C8FF
GRAY = 0x828282FF
function onSceneStart()
	print("Hello! Please Get the Player!");
	Ground = {
		x = 0, 
		y = Raylib.GetScreenHeight() - 50.0, 
		w = Raylib.GetScreenWidth(), 
		h = Raylib.GetScreenHeight() - 50.0,
		color = 0x00ff00ff
	}
	--[[
	Player.size = {x = 36, y = 36}
	Player.pos = {x = Ground.w * 0.5, y = Raylib.GetScreenHeight() * 0.5}
	Player.Floor = ground.y;
	Player.canJump = false;
	Player.speed = 0;
	]]
	local width = Raylib.GetScreenWidth()
	local height = Raylib.GetScreenHeight()
	envElements = {
        { x = 0,	y = 0,	 w=1000, h=400	, blocking =0, color = LIGHTGRAY },
        { x = 0,	y = 400, w=1000, h=200	, blocking =1, color = GRAY },
        { x = 300,	y = 200, w=400,  h=10   , blocking =1, color = GRAY },
        { x = 250,	y = 300, w=100,  h=10   , blocking =1, color = GRAY },
        { x = 650,	y = 300, w=100,  h=10   , blocking =1, color = GRAY }
	}
end
function onSceneEnd() end
function Update(dt) 
	--Player.Update(dt);
end
function Draw() 
	for i, rect in ipairs(envElements) do
		Raylib.DrawRectangle(rect.x, rect.y, rect.w, rect.h, rect.color)
	end

	Raylib.DrawLine(Ground.x, Ground.y, Ground.w, Ground.h, Ground.color);
	--Player.Draw();
end
function onKeyPress(key)
	--Player.HandleKeyPress(key);
end


