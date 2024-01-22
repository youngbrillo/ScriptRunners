function onSceneStart()
	GenTileMap();
end

function onSceneEnd() 

end
function Update(dt) 

end

function Draw() 
	drawGrid(0, 100, 10);
end

function UIDraw() 
end
function drawGrid(min, max, spacing)
	for y = min, max, spacing do
		for x = min, max, spacing do
			Raylib.DrawLine(x, y, max, y, 0xffffff7e)
		end
	end
end

function GenTileMap() 
	e = Scene.CreateTilemapNode("tile map")
end
