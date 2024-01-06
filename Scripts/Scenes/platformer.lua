
--Player = require("Scripts/Scenes/Player")
Player = {}
LIGHTGRAY = 0xC8C8C8FF
GRAY = 0x828282FF
function onSceneStart()
	print("Hello! Please Get the Player!");
	Ground = {
		x = 0, 
		y = 40, 
		w = 100, 
		h = 40,
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
        { x = 00,	y = 00,	w=100,  h=40 , blocking =false,color = LIGHTGRAY },
        { x = 00,	y = 40,	w=100,  h=20 , blocking =true, color = GRAY },
        { x = 30,	y = 20, w=040,  h=01 , blocking =true, color = GRAY },
        { x = 25,	y = 30, w=010,  h=01 , blocking =true, color = GRAY },
        { x = 65,	y = 30, w=010,  h=01 , blocking =true, color = GRAY }
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


