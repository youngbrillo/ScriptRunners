objs = {}
--//keys /////////////////////////////////
KEY_LEFT = 263
KEY_UP = 265
KEY_RIGHT = 262
KEY_DOWN = 264
KEY_SPACE = 32
KEY_TAB = 258
--//shape types ////////////////////////

Rectangle_Shape = 0;
Circle_Shape = 1;
Line_Shape = 2;

onDropMessages = 
{
	"Keep At it!",
	"Don't give up!",
	"Try, Try, Try Again",
	"Ooops!",
	"That's too bad!",
}

onLaunchMessages = 
{
	"You can do it!",
	"Almost there!",
	"Only so many left!"
}
local Demo = require("Scripts/Scenes/demoScene")

function onSceneStart()
	local _width = Raylib.GetScreenWidth() * 0.1;
	
	objs["Player"] = 
	{
		name = "Player",
		pos = {x = Raylib.GetScreenWidth() * 0.5 - _width * 0.5, y = Raylib.GetScreenHeight() * 0.875},
		size = {x = _width , y = 20},
		color = 0x505050ff,
		shape = Rectangle_Shape,
		speed = {x = 0, y = 0},
		life = 5,
		direction = 0,
		movementSpeed = Raylib.GetScreenWidth() * 0.75,
		brick = false,
		drawShadow = true
	}
	objs["Ball"] = 
	{
		pos = {x = Raylib.GetScreenWidth() * 0.5, y = Raylib.GetScreenHeight() * 0.875 - 30},
		size = {x = 7, y = 40},
		speed = {x = 0, y = 0},
		color = 0xff0000ff,
		shape = Circle_Shape,
		active = false,
		launchMag = Raylib.GetScreenWidth() * 0.75,
		brick = false,
		drawShadow = true

	}
	GenerateObjects(5, 20, {x = Raylib.GetScreenWidth() / 20, y = 40 }, 25, 0x55ff55ff, 0xffff55ff)

	print("number of objects: ".. #objs)
	
	gString = "press [SPACE] to start"
	gFontSize = 20.0;
	gFontColor = 0xFFFFFFFF
	padding = 10
	cursor = 0;
	cursorTimer = 0.0;
	cursorTimeMax = 0.07

	--goto:	AdvanceCursor()


	StringObject = 
	{
		text = "Level: 0",
		position = {x = 60, y = Raylib.GetScreenHeight() - 30- 50},
		fontSize = 40,
		spacing = 1.0,
		color = 0x7D7D7DFF
	}

	shadow_x = 10;
	shadow_y = 10;
	shadowColor = 0x000000ff;
	shadowColor = 0xffffffff;
	Scene.Inspect("padding", "gFontSize", "gString", "cursorTimeMax", "shadow_x", "shadow_y")-- "gStringObject", "objs")
	Node = test.Node();
	Demo.CreateCheckeredNodes(350)
end


function onSceneEnd()

end

function Update(dt)
	AdvanceCursor(dt);
	UpdatePlayer(dt);
	UpdateBall();
	for k, v in pairs(objs) do
		UpdateObject(v, dt);
	end
end


function Draw()

	for k, v in pairs(objs) do
		DrawObject(v);
	end

	DrawLife(Player().life)
	local vx, vy = Raylib.MeasureText(gString, gFontSize)
	Raylib.DrawText(string.sub(gString, 0, cursor), Raylib.GetScreenWidth() - (vx + padding), Raylib.GetScreenHeight() - (vy + padding), gFontSize, gFontColor)
	Raylib.DrawText(StringObject.text, StringObject.position.x, StringObject.position.y, StringObject.fontSize, StringObject.color)
	
end



function onKeyPress(key)
	basicPlayerMovement(key);
end


-- CUSTOM FUNCTIONS ////////////////////////////////////////////////////////////////
function ResetCursor()
	cursor = 0;
	cursorTimer = 0.0;
end


function AdvanceCursor(dt)
	if cursor < string.len(gString) then 
		cursorTimer = cursorTimer + dt;
		if(cursorTimer > cursorTimeMax) then
			cursor = cursor + 1;
			cursorTimer = 0.0;
		end
	end
end



function GenerateObjects(rows, columns, size, offset, colorA, colorB)
	count = 1;
	for y = 0, rows-1, 1 do
		for x =0, columns-1, 1 do
			objs[count] = {
				pos = {
					x = x * size.x, 
					y = y * size.y + offset
				},
				size = size,
				color = ((x + y) % 2 == 0) and  colorA or colorB,
				shape = Rectangle_Shape,
				speed = {x = 0, y = 0},
				brick = true,
				drawShadow = true

			}

			count = count + 1;
		end
	end
end


-- //QUICK GETTERS //////////////////////////////////////////////
function Player() return objs["Player"] end
function Ball() return objs["Ball"] end


-- //DRAW FUNCTIONS ////////////////////////////////////////////

function DrawObject( obj )
	if(obj.shape == Rectangle_Shape) then -- draw Rectangle
		if obj.drawShadow then Raylib.DrawRectangle(obj.pos.x + shadow_x, obj.pos.y + shadow_y, obj.size.x, obj.size.y, shadowColor) end
		Raylib.DrawRectangle(obj.pos.x, obj.pos.y, obj.size.x, obj.size.y, obj.color);

	elseif (obj.shape == Circle_Shape) then -- draw Circle
		if obj.drawShadow then Raylib.DrawCircle(obj.pos.x + shadow_x, obj.pos.y + shadow_y, obj.size.x, shadowColor) end
		Raylib.DrawCircle(obj.pos.x, obj.pos.y, obj.size.x, obj.color);

	elseif (obj.shape == Line_Shape) then -- draw Line
		if obj.drawShadow then Raylib.DrawRectangle(obj.pos.x + shadow_x, obj.pos.y + shadow_y, obj.size.x + shadow_x, obj.size.y + shadow_x, shadowColor) end
		Raylib.DrawLine(obj.pos.x, obj.pos.y, obj.size.x, obj.size.y, obj.color);
	end
end

function DrawLife( count )
	for i = 1, count, 1 do  
		Raylib.DrawRectangle (20 + 40 * i, Raylib.GetScreenHeight() - 30, 35, 10, 0xC8C8C8FF);
	end
end


-- //UPDATE FUNCTIONS ////////////////////////////////////////////

function UpdateObject( obj, delta )
	obj.pos.x = obj.pos.x + obj.speed.x * delta;	
	obj.pos.y = obj.pos.y + obj.speed.y * delta;	
end

function basicPlayerMovement(key)
--[[

	if(key == KEY_DOWN) then
		Player().pos.y = Player().pos.y + Player().size.y;
	elseif key == KEY_UP then
		Player().pos.y = Player().pos.y - Player().size.y;
	elseif key == KEY_LEFT then
		Player().pos.x = Player().pos.x - Player().size.x;
	elseif key == KEY_RIGHT then
		Player().pos.x = Player().pos.x + Player().size.x;
]]
	if key == KEY_SPACE and not Ball().active then
		Ball().active = true;
		Ball().speed.y = -Ball().launchMag;
		gString = onLaunchMessages[math.random(1, #onLaunchMessages)]
		ResetCursor();
	else
		--print(">>>", key);
	end
end

function UpdatePlayer(dt)
	Player().direction = 0;

	if Raylib.IsKeyDown(KEY_LEFT) then Player().direction = Player().direction - 1 end
	if Raylib.IsKeyDown(KEY_RIGHT) then Player().direction = Player().direction + 1  end
	
    Player().pos.x =  Player().pos.x + Player().direction * Player().movementSpeed * dt;

    if ((Player().pos.x) < 0) then
        Player().pos.x = 0--Player().size.x * 0.5
    elseif ((Player().pos.x + Player().size.x) > Raylib.GetScreenWidth()) then
         Player().pos.x = Raylib.GetScreenWidth() -  Player().size.x;
	end
end





function UpdateBall()
	-- ball handling
	if Ball().active then
		handleCollisionWalls()
		handleCollisionPlayer();

		for k, v in pairs(objs) do
			if v.brick then 
				handleCollisionBrick(v, k);
			end 
		end
	else 
		Ball().pos.x = Player().pos.x + Player().size.x * 0.5;--, y = Raylib.GetScreenHeight() * 0.875 - 30}
	end
end

function handleCollisionPlayer()
    -- Collision logic: ball vs player
    if Raylib.CheckCollisionCircleRec(
			Ball().pos.x, 
			Ball().pos.y, 
			Ball().size.x,
			Player().pos.x ,-- - Player().size.2, 
			Player().pos.y ,-- - Player().size.y / 2, 
			Player().size.x, 
			Player().size.y
        )
	then
        if (Ball().speed.y > 0)
        then
            Ball().speed.y = Ball().speed.y * -1;
            Ball().speed.x = (Ball().pos.x - Player().pos.x) / (Player().pos.x / 2) * Ball().launchMag;
        end
    end
end


function handleCollisionWalls()
	if Ball().pos.x + Ball().size.x >= Raylib.GetScreenWidth() or Ball().pos.x - Ball().size.x <= 0 then
		Ball().speed.x = Ball().speed.y * -1;
	end
	if Ball().pos.y - Ball().size.x  <= 0 then
		Ball().speed.y = Ball().speed.y * -1;
	end
	if Ball().pos.y + Ball().size.x >= Raylib.GetScreenHeight() then
	
        Ball().speed = {x = 0, y = 0};
        Ball().active = false;
		Ball().pos.y = Raylib.GetScreenHeight() * 0.875 - 30;
        Player().life = Player().life - 1;
		gString = onDropMessages[math.random(1, #onDropMessages)]
		ResetCursor();
	end

end

function handleCollisionBrick(brick , key)
	if Raylib.CheckCollisionCircleRec(
		Ball().pos.x, 
		Ball().pos.y, 
		Ball().size.x,
		brick.pos.x ,-- brick.size.x / 2, 
		brick.pos.y ,-- brick.size.y / 2, 
		brick.size.x, 
		brick.size.y
    )
	then
		--print("Destroy this brick!!")
		objs[key] = nil;
		Ball().speed.x = Ball().speed.x * -1;
		Ball().speed.y = Ball().speed.y * -1;
	end
end
