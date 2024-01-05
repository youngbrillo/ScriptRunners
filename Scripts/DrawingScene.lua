objs = {


}

KEY_LEFT = 263
KEY_UP = 265
KEY_RIGHT = 262
KEY_DOWN = 264

Rectangle_Shape = 0;
Circle_Shape = 1;
Line_Shape = 2;


function onSceneStart()
	objs["Player"] = 
	{
		name = "Player",
		pos = {x = Raylib.GetScreenWidth() * 0.5, y = Raylib.GetScreenHeight() * 0.75},
		size = {x = Raylib.GetScreenWidth() * 0.1, y = 40},
		color = 0x00ffffff,
		shape = Rectangle_Shape
	}
	GenerateObjects(2, 20, {x = Raylib.GetScreenWidth() / 20, y = 40 }, 50, 0x555555ff, 0xffffffff)

	print("number of objects: ".. #objs)
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
				shape = Rectangle_Shape
			}

			count = count + 1;
		end
	end
end



function onSceneEnd()

end

function Update(dt)

end


function Draw()

	for k, v in pairs(objs) do
		DrawObject(v);
	end
end

function DrawObject( obj )
	if(obj.shape == Rectangle_Shape) then
		Raylib.DrawRectangle(obj.pos.x, obj.pos.y, obj.size.x, obj.size.y, obj.color);
	elseif (obj.shape == Circle_Shape) then
		Raylib.DrawCircle(obj.pos.x, obj.pos.y, obj.size.x, obj.color);
	elseif (obj.shape == Line_Shape) then
		Raylib.DrawLine(obj.pos.x, obj.pos.y, obj.size.x, obj.size.y, obj.color);
	end
end

function onKeyPress(key)
	if(key == KEY_DOWN) then
		objs["Player"].pos.y = objs["Player"].pos.y + objs["Player"].size.y;
	elseif key == KEY_UP then
		objs["Player"].pos.y = objs["Player"].pos.y - objs["Player"].size.y;
	elseif key == KEY_LEFT then
		objs["Player"].pos.x = objs["Player"].pos.x - objs["Player"].size.x;
	elseif key == KEY_RIGHT then
		objs["Player"].pos.x = objs["Player"].pos.x + objs["Player"].size.x;
	end
end

