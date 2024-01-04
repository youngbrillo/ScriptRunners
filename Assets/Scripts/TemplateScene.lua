

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
		Ball:ListenForCollisions();
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
	player_Speed = Raylib.GetScreenWidth() * 0.75
	ball_active = false;
	launchMagnitude = Raylib.GetScreenWidth() * 0.75;
end

KEY_LEFT = 263;
KEY_RIGHT = 262;
KEY_ENTER = 32;
function onKeyPress(key)
	--print(">>>", key)
	if isBreakOut then
		if key == KEY_ENTER then
			LaunchBall();
		elseif key == KEY_LEFT then
			Player.velocity.x = -player_Speed;
			if not ball_active then Ball.velocity.x = -player_Speed; end
		elseif key == KEY_RIGHT then
			Player.velocity.x = player_Speed;
			if not ball_active then Ball.velocity.x = player_Speed; end
		end
	end
end

function onSimpleCollision(A, B, x, y)
	if A.name == Ball.name then
		--print("collide away: ", node.name, x, y)
		Ball.velocity:set(x, y);
		if B.name ~= Player.name then 
			B.canDestroy = true;
		end
	end
end



function LaunchBall()
	if not ball_active then
		Ball.velocity:set(0, -launchMagnitude);
		ball_active = true;
	end
end
