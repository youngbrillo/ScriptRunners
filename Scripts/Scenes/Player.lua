KEY_LEFT = 263
KEY_UP = 265
KEY_RIGHT = 262
KEY_DOWN = 264
KEY_SPACE = 32

local Player = {};

Player.pos = {x = 0, y = 0}
Player.size = {x = 1, y = 1}
Player.direction = {x = 0, y = 0};
Player.speed = 500;
Player.color = 0xffffffff;
Player.isInCameraSpace = false;
Player.Floor = nil;
Player.Gravity = 1000;
function Player.Draw() 
	Raylib.DrawRectangle(Player.pos.x, Player.pos.y, Player.size.x, Player.size.y, Player.color);
end

function Player.Update(dt)
	Player.ApplyVelocity(dt);
	Player.HandleInput();

	if not Player.isInCameraSpace then
		Player.ConstrainToScreen();
	end
end
function Player.ApplyVelocity( dt )
	verticalForce = Player.direction.y * Player.speed;
	if(not Player.Grounded()) then 
		verticalForce = Player.Gravity;
	end
	

	Player.pos.x = Player.pos.x + Player.direction.x * Player.speed * dt;
	Player.pos.y = Player.pos.y + verticalForce * dt;


end


function Player.HandleKeyPress(key)
	-- body
	--if(key == KEY_UP) then Player.direction
end


function Player.HandleInput()
	Player.direction = {x = 0, y = 0};
	if Raylib.IsKeyDown(KEY_LEFT) then Player.direction.x = Player.direction.x - 1 end
	if Raylib.IsKeyDown(KEY_RIGHT) then Player.direction.x = Player.direction.x + 1  end
	if Raylib.IsKeyDown(KEY_UP) then Player.direction.y = Player.direction.y - 1  end
	
end

-- Helpers /////////////////////
function Player.ConstrainToScreen()
	if(Player.pos.x < 0) then Player.pos.x = 0; end
	if(Player.pos.x + Player.size.x > Raylib.GetScreenWidth()) then Player.pos.x = Raylib.GetScreenWidth() - Player.size.x; end
	if(Player.pos.y + Player.size.y > Raylib.GetScreenHeight()) then Player.GetFloor();  end
	if(Player.pos.x < 0) then Player.pos.x = 0; end
end

function Player.GetFloor()
	local val = 0;
	if(Player.Floor ~= nil) then
		val = Player.Floor  - Player.size.y;
	else
		val = Raylib.GetScreenHeight() - Player.size.y;
	end

	return val
end

function Player.Grounded( )
	return Player.pos.y + Player.size.y >= Player.GetFloor();
end

return Player;