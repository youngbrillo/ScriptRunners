
--Player = require("Scripts/Scenes/Player")
Player = {}
LIGHTGRAY = 0xC8C8C8FF
GRAY = 0x828282FF
function onSceneStart()

	envSpan = 100;
	envHeight = 40;
	print("Hello! Please Get the Player!");
	Ground = {
		x = 0, 
		y = 40, 
		w = envSpan, 
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
       -- { x = 00,	y = 00,	w=envSpan,  h=envHeight , blocking =false,color = LIGHTGRAY },
        { x = 00,	y = envHeight,	w=envSpan,  h=20 , blocking =true, color = GRAY },
        { x = 30,	y = 20, w=040,  h=01 , blocking =true, color = GRAY },
        { x = 25,	y = 30, w=010,  h=01 , blocking =true, color = GRAY },
        { x = 65,	y = 30, w=010,  h=01 , blocking =true, color = GRAY }
	}



	App.GetCamera().zoom = 8;
	App.GetCamera().target:set(envSpan * 0.5, envHeight - 10);

	local p = Scene.CreateNode2d("Player");
	p.transform.size.x = envSpan;
	p.transform.size.y = envHeight;
	p.transform:Align(0, 0);
	p.material:SetColor(LIGHTGRAY)
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


