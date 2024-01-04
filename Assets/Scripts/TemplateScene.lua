

function onSceneStart()
	print("Hello from LUA!! we will be making some nodes today!")
	Node = test.Node();
	math.randomseed(os.time());
	--createCheckeredNodes(350);
	createBreakOut();

end

function onSceneEnd()
	print("Goodbye from LUA!!")

end

function createCheckeredNodes( limit )
	for i=1, limit, 1 do
		local x = math.random(0, Raylib.GetScreenWidth())
		local y = math.random(0, Raylib.GetScreenHeight())
		local size = math.random(15, 100); size = size + math.random();
		local ang = math.random(-360, 360)
		local uvx = math.random(-10, 10);
		local uvy = math.random(-10, 10);
		local av = math.random(-60, 60);
		local name = "node " .. i;
			-- node creation 
		local e = Node:Create(name, x, y, size, size);

			-- material properties
		e.material:SetColorVec(math.random(), math.random(), math.random(), 0.66);
		e.material:SetTextureByAlias("CHECKER");
		e.material.uv_scroll:set(uvx, uvy);
			-- transform properties
		e.transform.angle = ang;

		
			-- node properties
		e.AngularVelocity = av;
		e.velocity:set(math.random(-10, 10),  math.random(-10, 10));
			-- attach to script table
	end
end


function createBreakOut()
	local LINES_OF_BRICKS = 5;
	local BRICKS_PER_LINE = 20;
    brickSize = {x = Raylib.GetScreenWidth() / BRICKS_PER_LINE, y = 40 };
	
    local screenWidth = Raylib.GetScreenWidth();
    local screenHeight = Raylib.GetScreenHeight();
    local offset = 50;


	Player = Node:Create("Player", screenWidth * .5, screenHeight * 0.85, screenWidth/10, 20);
	Ball = Node:Create("Ball", screenWidth * .5, screenHeight * 0.85 - 30, 7.0, 0.1);
		Ball.transform.shape_type = 1;
		Ball.material:SetColor(0xff0000ff);

    for i= 0, LINES_OF_BRICKS - 1, 1 do
        for  j = 0, BRICKS_PER_LINE - 1, 1 do
			local name = "Brick: ".. i  .. "-" .. j;

			local e = Node:Create(name, 
				j * brickSize.x + brickSize.x / 2, 
				i * brickSize.y + offset,
				brickSize.x,
				brickSize.y
			);

			if ((i + j) % 2 == 0) then 
				e.material:SetColorVec(200/255, 200/255, 200/255, 1.0)
			else
				e.material:SetColorVec(80/255, 80/255, 80/255, 1.0)

			end

			e.transform.shape_type = 0;

        end
    end

	isBreakOut = true;

end