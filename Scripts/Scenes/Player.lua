KEY_LEFT = 263
KEY_UP = 265
KEY_RIGHT = 262
KEY_DOWN = 264
KEY_SPACE = 32

text = "Player Controller v.00"

local Player =  {
	speed = {x = 0, y = 0}
}

local trigger_helper =  {

}


function Player.Create()
	Player.p = Scene.CreateNode2d("Player");
	Player.p.transform.size:set(0.5, 1);
	Player.p.speed = 20;

	
	move_to_x = Player.p.transform.position.x
	move_to_y = Player.p.transform.position.y
	move_speed_h = 0
	move_speed_v = 0
	
	clamp_walk = 30 --// maximum walking speed 
	clamp_fall_speed = 20

	walk_acceleration = 20 --// when holding button how fast to get to max speed
	walk_decceleration = 1.4 --// when no button is held how fast to get to 0

	jump_height = 19
	jump_early_end = false

	current_h_speed = 0 --// how fast the player would like to move 
	current_v_speed = 0 

	fall_speed = 1.2
	fall_speed_min = 0.8
	fall_speed_max = 1.2

	grounded = false
	touched_ground_since_last_jump = false

	--// recording how many frames ago these things happened 
	grounded_frames_ago = 999
	landed_frames_ago = 999
	apex_frames_ago = 999 --// 0 if on floor and on way up (only counts on way down)
	w_frames_ago = 999
	frames_going_down = 999

	at_apex = false --// this isnt the apex but the top 4th
	speed_for_apex = 6 --// if they are going slower than this and in the air we call this the apex



end

function Player.Draw() 
	--Raylib.DrawText(string.sub(text, 0, cursor), Raylib.GetScreenWidth() - (vx + padding), Raylib.GetScreenHeight() - (vy + padding), gFontSize, gFontColor)
	local hText = text;
	hText = hText .. "\n Current speed: ".. current_h_speed;
	for k, v in ipairs(trigger_helper) do 
		hText = hText.. "\n" .. k .. ": ".. v;
	end

	local vx, vy = Raylib.MeasureText(hText, 20)
	Raylib.DrawText(hText, Raylib.GetScreenWidth() - (vx + 10), Raylib.GetScreenHeight() - (vy + 20), 20, 0xffffffff)



end

function Player.Update(dt)
	--HandleInput();
	
	w = keyboard_check_pressed(KEY_UP)
	w_end = keyboard_check_released(KEY_UP)
	a = keyboard_check(KEY_LEFT) and 1 or 0
	s = keyboard_check(KEY_DOWN) 
	d = keyboard_check(KEY_RIGHT) and 1 or 0	


	if (a > 0 or d > 0) then -- // they are moving left / right
		current_h_speed = current_h_speed + (d-a)*(walk_acceleration*dt)
		current_h_speed = clamp(current_h_speed,-(clamp_walk*dt),(clamp_walk*dt))
	else -- // not moving 
		--//slow them down 
		if (current_h_speed > walk_decceleration) then 
			current_h_speed = current_h_speed- (walk_decceleration*dt)
		elseif (current_h_speed < -walk_decceleration) then
			current_h_speed = current_h_speed- (walk_decceleration*dt)
		else 
			current_h_speed = 0

		end
	end

	-- //Am I at the Apex ??
	at_apex = false
	fall_speed = fall_speed_max
	if (grounded == false) then
		if (math.abs(current_v_speed) < speed_for_apex) then
			at_apex = true
			fall_speed = fall_speed_min
			--trigger_helper(helper.anti_gravity_apex)
			trigger_helper["anti gravity apex"] = true;
		end
	end
	
	--// gravty 
	current_v_speed	= current_v_speed + (fall_speed/dt)
	--// max fall speed 
	if (current_v_speed > clamp_fall_speed ) then --// and helper_on[helper.clamp_fall_speed])  then
		current_v_speed = clamp_fall_speed
		trigger_helper["clamp fall speed"] = true; 
	end

	--// work out if I am allowed to jump
	local can_jump = false 
	if (grounded == true) then 
		can_jump = true; 	 
	elseif (grounded_frames_ago < 8 and grounded == false ) then 
		can_jump = true;
	elseif (touched_ground_since_last_jump == false) then 
		can_jump = false ;
	end
	
	local use_w = w --// for buffering w
	if (w_frames_ago < 7 and frames_going_down > 2 )then --//and helper_on[helper.jump_buffering])  then // can only jump buffer after going down for a little
		use_w = true
	
		if (can_jump) then
			trigger_helper["jump_buffering"] = true;
		end
	end

	--// DO THE JUMP
	if (use_w and can_jump)  then
		current_v_speed = -(jump_height)
		touched_ground_since_last_jump = false
		jump_early_end = false

		if (grounded == false)  then
			--// coyote_jump was used
			trigger_helper["coyote_jump"] = true;
		end
	end

	--// Better movment at apex
	if (at_apex  )then 
		if (current_h_speed > 1 and a)  then
			current_h_speed = current_h_speed - Player.p.speed
			trigger_helper["speedy_apex"] = true;
		end
		if (current_h_speed < -1 and d)  then
			current_h_speed = current_h_speed +  Player.p.speed
			trigger_helper["speedy_apex"] = true
		end
	
	end

	--// early end jump 
	if (w_end and grounded == false and jump_early_end == false and current_v_speed < 0 )  then
		jump_early_end = true
	
		current_v_speed = 0
		trigger_helper["early_fall"] = true;
	end



	
	if not grounded then
		landed_frames_ago = 00
	else
		grounded = true;
		grounded_frames_ago = 0;
		current_v_speed = 00
		touched_ground_since_last_jump = true;

	end

	local  move_to_x = Player.p.transform.position.x + (current_h_speed*dt)
	local  move_to_y = Player.p.transform.position.y + (current_v_speed*dt)




	
	Player.p.transform.position:set(move_to_x, move_to_y);
	Player.speed = {x = current_h_speed, y = current_v_speed };

	--[[
	]]

end

function Player.HandleKeyPress(key)
	--print("Pressed Key: "..key)
end


function HandleInput()
	Player.p.direction:set(0.0, 0.0)
	if Raylib.IsKeyDown(KEY_LEFT) then Player.p.direction.x = Player.p.direction.x - 1 end
	if Raylib.IsKeyDown(KEY_RIGHT) then Player.p.direction.x = Player.p.direction.x + 1  end
end

-- Helpers /////////////////////



function keyboard_check_pressed(key) return Raylib.IsKeyPressed(key) end;
function keyboard_check_released(key) return Raylib.IsKeyReleased(key) end;
function keyboard_check(key) return Raylib.IsKeyDown(key) end;

function clamp(val, lower, upper)
    assert(val and lower and upper, "not very useful error message here")
    if lower > upper then lower, upper = upper, lower end -- swap if boundaries supplied the wrong way
    return math.max(lower, math.min(upper, val))
end

function Player.setGrounded(val)
	grounded = val;

end


return Player;
