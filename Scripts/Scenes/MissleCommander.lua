-- hooked functions -- /////////////////////////////////////////////////////////////////////////////////////
function onSceneStart()
	InitalizeGame();
end

function onSceneEnd() 

end
function Update(dt) 
	HandleGameRules(dt);
end

function Draw() 
end

function UIDraw() 
    DrawGamePlayScene();
end

function onKeyPress(key)
    HandleMCKeyEvents(key);
end
-- my functions -- /////////////////////////////////////////////////////////////////////////////////////

KEY_FIRE = 32   -- //KEY_SPACE
KEY_FIRE_1 = 90 -- //KEY_Z
KEY_FIRE_2 = 88 -- //KEY_X
KEY_FIRE_3 = 67 -- //KEY_C
KEY_ENTER = 257;
KEY_ENTER2 = 335;
function HandleMCKeyEvents(key)

    if(not gameOver)
    then
        if(key == KEY_FIRE_1) then 
            fire1 = true;
        elseif(key == KEY_FIRE_2 or key == KEY_FIRE) then 
            fire2 = true;
        elseif(key == KEY_FIRE_3) then 
            fire3 = true;
        end
    else
        if key == KEY_ENTER or key == KEY_ENTER2 then
            InitalizeGame()
        elseif(key == 258) then --//tab key
            App.GoToScene("Platforming")
        else 
            print(key);
        end
    end
end


function InitalizeGame()

	framesCounter = 0;
    gameOver = false;
    score = 0;
    missile = { }
    interceptor = { }
    explosion = {}
    launcher  = {}
    building  = {}
    explosionIndex = 1;

    fire1 = false;
	fire2 = false;
	fire3 = false;
	MAX_MISSILES                =100
	MAX_INTERCEPTORS            =30
	MAX_EXPLOSIONS              =100
	LAUNCHERS_AMOUNT            =3           --// Not a variable, should not be changed
	BUILDINGS_AMOUNT            =6           --// Not a variable, should not be changed
	LAUNCHER_SIZE               =80
	BUILDING_SIZE               =60
	EXPLOSION_RADIUS            =40
	MISSILE_SPEED               =1
	MISSILE_LAUNCH_FRAMES       =30
	INTERCEPTOR_SPEED           =10
	EXPLOSION_INCREASE_TIME     =90          --// In frames
	EXPLOSION_TOTAL_TIME        =210         --// In frames

	EXPLOSION_COLOR              =0x7D7D7D7D;
		
	startPhase();
	math.randomseed(os.time());
    interceptorNumber = 1;

end

function startPhase()

    --// Initialize missiles
	for i = 1, MAX_MISSILES, 1 do
		missile[i] = {
			origin = {x = 0, y = 0},
			objective = {x = 0, y = 0},
			speed = {x = 0, y = 0},
			position = {x = 0, y = 0},
			active = false
		}
	end

    --// Initialize interceptors
	for i = 1, MAX_INTERCEPTORS, 1 do
        interceptor[i] = {
			origin = { x=0, y=0 }
			, speed = { x=0, y=0}
			, position = { x=0, y=0 }
			, active = false
		}
	end

    --// Initialize explosions
	for i = 1, MAX_EXPLOSIONS, 1 do

        explosion[i] = {
			position = { x=0, y=0}
			, frame = 0
			, active = false
			, radiusMultiplier = 1.0
		}
	end
	
    --// Initialize launchers
	for i = 1, LAUNCHERS_AMOUNT, 1 do

        launcher[i] = {
			position = { x=0, y=0}
			, active = false
		}
	end

	
    --// Initialize launchers
	for i = 1, BUILDINGS_AMOUNT, 1 do

        building[i] = {
			position = { x=0, y=0}
			, active = false
		}
	end
    local spacing = Raylib.GetScreenWidth() / (LAUNCHERS_AMOUNT + BUILDINGS_AMOUNT + 1);
	
    --// Buildings and launchers placing
    launcher[0+1].position = { x= 1.0 * spacing, y=Raylib.GetScreenHeight() - LAUNCHER_SIZE / 2 };
    building[0+1].position = { x= 2.0 * spacing, y=Raylib.GetScreenHeight() - BUILDING_SIZE / 2 };
    building[1+1].position = { x= 3.0 * spacing, y=Raylib.GetScreenHeight() - BUILDING_SIZE / 2 };
    building[2+1].position = { x= 4.0 * spacing, y=Raylib.GetScreenHeight() - BUILDING_SIZE / 2 };
    launcher[1+1].position = { x= 5.0 * spacing, y=Raylib.GetScreenHeight() - LAUNCHER_SIZE / 2 };
    building[3+1].position = { x= 6.0 * spacing, y=Raylib.GetScreenHeight() - BUILDING_SIZE / 2 };
    building[4+1].position = { x= 7.0 * spacing, y=Raylib.GetScreenHeight() - BUILDING_SIZE / 2 };
    building[5+1].position = { x= 8.0 * spacing, y=Raylib.GetScreenHeight() - BUILDING_SIZE / 2 };
    launcher[2+1].position = { x= 9.0 * spacing, y=Raylib.GetScreenHeight() - LAUNCHER_SIZE / 2 };

	
    for i = 1, LAUNCHERS_AMOUNT, 1 do launcher[i].active = true; end
    for i = 1, BUILDINGS_AMOUNT, 1 do building[i].active = true; end

    score = 0;

    missileIndex = 1;
    cursorPos = {x = 0, y = 0};
    fire1 = false;
    fire2 = false;
    fire3 = false;
end

function DrawGamePlayScene()
	if(gameOver == false) then
		DrawInProg()
	    DrawInstructions();
	else
        local patext = "PRESS [ENTER] TO PLAY AGAIN"
        Raylib.DrawText(patext, 
        Raylib.GetScreenWidth()/2 - Raylib.MeasureText(patext, 20)/2, 
        Raylib.GetScreenHeight()/2 - 50, 
        20, 
        0x5d5d5dff);
    end
end

function DrawInProg()
	-- body
	
    --// Draw missiles
    for i, miss in ipairs(missile) do
        if (miss.active) then
            Raylib.DrawLine(miss.origin.x, miss.origin.y, miss.position.x, miss.position.y, 0xff0000ff);
            if (framesCounter % 16 < 8) then
				Raylib.DrawCircle(miss.position.x, miss.position.y, 3, 0xffff00ff);
			end
        end
    end

    --// Draw interceptors
    for i, int in ipairs(interceptor) do
        if (int.active) then
            Raylib.DrawLine(int.origin.x, int.origin.y, int.position.x, int.position.y, 0x00ffffff);

            if (framesCounter % 16 < 8) then 
				Raylib.DrawCircle(int.position.x, int.position.y, 3, 0x0000ffff); 
			end
        end
    end

    --// Draw explosions
    for i, exp in ipairs(explosion) do
        if (exp.active) then 
			Raylib.DrawCircle(exp.position.x, exp.position.y, EXPLOSION_RADIUS * exp.radiusMultiplier, EXPLOSION_COLOR);
		end
	end
    --// Draw buildings and launchers
    for i, lnchr in ipairs(launcher) do
        if (lnchr.active) then
			Raylib.DrawRectangle(lnchr.position.x - LAUNCHER_SIZE/2, lnchr.position.y - LAUNCHER_SIZE/2, LAUNCHER_SIZE, LAUNCHER_SIZE, 0x5d5d5dff);
		end
	end

    for i, build in ipairs(building) 
    do
        if (build.active)  then
			Raylib.DrawRectangle(build.position.x - BUILDING_SIZE/2, build.position.y - BUILDING_SIZE/2, BUILDING_SIZE, BUILDING_SIZE, 0xdededeff);
		end
    end


    drawCursor();
end


function DrawInstructions()
    Raylib.DrawText("SCORE ".. score, 20, 20, 40, 0xC87DC8FF);
    --Raylib.DrawText("interceptor Number ".. interceptorNumber, 20, 50, 20, 0xffffffff);
    --Raylib.DrawText("explosion Number ".. explosionIndex, 20, 70, 20, 0xffffffff);
	--local vx, vy = Raylib.MeasureText(inText, 20)
	--Raylib.DrawText(inText, Raylib.GetScreenWidth() - (vx + 10), Raylib.GetScreenHeight() - (vy + 20), 20, 0xffffffff)

    if(App.isPaused()) then
        local mtext = "PRESS [P] TO UNPAUSE"
        local vx, vy = Raylib.MeasureText(mtext, 20)
	    Raylib.DrawText(mtext, Raylib.GetScreenWidth()/2 - vx /2 , Raylib.GetScreenHeight()/2, 20, 0xffffffff)
    end
end


function HandleGameRules(dt)
	if(gameOver == false) then
	    framesCounter = framesCounter + 1;
		update_Interceptors(dt);
		update_Missiles(dt);
		update_Explosions(dt);

		UpdateOutgoingFire();
		UpdateIncomingFire();
        
        --// Game over logic
        local checker = 0;

        for i = 1, LAUNCHERS_AMOUNT, 1
        do
            if (not launcher[i].active) then checker = checker + 1; end
            if (checker == LAUNCHERS_AMOUNT) then gameOver = true; end
        end
        checker = 0;
        for i = 1, BUILDINGS_AMOUNT, 1
        do
            if (not building[i].active) then checker = checker + 1; end
            if (checker == BUILDINGS_AMOUNT) then gameOver = true; end
        end
    end
end

function update_Interceptors(delta)

    --// Interceptors update
    for i = 1, MAX_INTERCEPTORS, 1
    do
        if (interceptor[i].active)
        then
            --// Update position
            interceptor[i].position.x = interceptor[i].position.x + interceptor[i].speed.x;
            interceptor[i].position.y = interceptor[i].position.y + interceptor[i].speed.y;

            --// Distance to objective
            local distance = math.sqrt(
                (interceptor[i].position.x - interceptor[i].objective.x) ^ 2+
                (interceptor[i].position.y - interceptor[i].objective.y) ^ 2
            );

            if (distance < INTERCEPTOR_SPEED)
            then
                --// Interceptor dissapears
                interceptor[i].active = false;

                --// Explosion
                explosion[explosionIndex].position =  {
                    x = interceptor[i]
                        .position.x, 
                    y = interceptor[i]
                        .position.y
                };
                explosion[explosionIndex].active = true;
                explosion[explosionIndex].frame = 0;
                explosionIndex = explosionIndex+1;
                if (explosionIndex == MAX_EXPLOSIONS) then explosionIndex = 1; end

                break;
            end
        end
    end
end

function update_Missiles(delta)

	for i,v in ipairs(missile) do
        if (missile[i].active)
        then
            --// Update position
            missile[i].position.x = missile[i].position.x + missile[i].speed.x;
            missile[i].position.y = missile[i].position.y + missile[i].speed.y;

            --// Collision and missile out of bounds
            if (missile[i].position.y > Raylib.GetScreenHeight()) 
            then 
                missile[i].active = false;
            else
                handleLauncherCollisions(i);
                handleBuildingCollisions(i);
                handleExplosionCollisions(i);
            end
        end
	end

end

function update_Explosions(delta)
    --// Explosions update
    for i = 1, MAX_EXPLOSIONS, 1
    do
        if (explosion[i].active)
        then
            explosion[i].frame = explosion[i].frame + 1;

            if (explosion[i].frame <= EXPLOSION_INCREASE_TIME) 
            then
                explosion[i].radiusMultiplier = explosion[i].frame / EXPLOSION_INCREASE_TIME;
            elseif (explosion[i].frame <= EXPLOSION_TOTAL_TIME)  then
                explosion[i].radiusMultiplier = 1 - (explosion[i].frame - EXPLOSION_INCREASE_TIME) / EXPLOSION_TOTAL_TIME;
            else
                explosion[i].frame = 0;
                explosion[i].active = false;
            end
        end
    end
end

function handleLauncherCollisions(i)
    --// CHeck collision with launchers
    for j = 1,LAUNCHERS_AMOUNT, 1
    do
        if (launcher[j].active)
        then
            if (getMissileCollision(missile[i], launcher[j], LAUNCHER_SIZE))
            then
                handleOnCollision(missile[i], launcher[j]);
                break;
            end
        end
    end
end

function handleBuildingCollisions(i)
    --// CHeck collision with buildings
    for j = 1,BUILDINGS_AMOUNT, 1
    do
        if (building[j].active)
        then
            if (getMissileCollision(missile[i], building[j], BUILDING_SIZE))
            then
                handleOnCollision(missile[i], building[j]);
                break;
            end
        end
    end
end


function getMissileCollision(mMissile, mObject, mSize)
    local x = mMissile.position.x;
    local y = mMissile.position.y;
    local rx = mObject.position.x - mSize/2
    local ry = mObject.position.y - mSize/2;
    local rw = mSize;
    local rh = mSize;

    return Raylib.CheckCollisionPointRec(x, y, rx, ry, rw, rh);
end

function handleOnCollision(mMissile, mObject)
    --// Missile dissapears
    mMissile.active = false;
    --// Explosion and destroy building
    mObject.active = false;

    explosion[explosionIndex].position = {x = mMissile.position.x, y= mMissile.position.y};
    explosion[explosionIndex].active = true;
    explosion[explosionIndex].frame = 0;
    explosionIndex = explosionIndex + 1;
    if (explosionIndex == MAX_EXPLOSIONS) then 
        explosionIndex = 1; 
    end
end


function handleExplosionCollisions(i)
    --// CHeck collision with explosions
    for j = 1, MAX_EXPLOSIONS, 1
    do
        if (explosion[j].active)
        then
            if (Raylib.CheckCollisionPointCircle(
                missile[i].position.x, 
                missile[i].position.y, 
                explosion[j].position.x, 
                explosion[j].position.y, 
                EXPLOSION_RADIUS * explosion[j].radiusMultiplier
            ))
            then
                --// Missile dissapears and we earn 100 points
                missile[i].active = false;
                score = score + 100;

                explosion[explosionIndex].position = missile[i].position;
                explosion[explosionIndex].active = true;
                explosion[explosionIndex].frame = 0;
                explosionIndex = explosionIndex + 1;
                if (explosionIndex == MAX_EXPLOSIONS) then explosionIndex = 1 end

                break;
            end
        end
    end
end
function UpdateOutgoingFire()
    local launcherShooting = 0;

    if (fire1) then launcherShooting = 1 end
    if (fire2) then launcherShooting = 2 end
    if (fire3) then launcherShooting = 3 end

    --interceptorNumber = launcherShooting;
    if (launcherShooting > 0 and launcher[launcherShooting].active)
    then
        local module = 0;
        local sideX = 0;
        local sideY = 0;

        --// Activate the interceptor
        interceptor[interceptorNumber].active = true;

        --// Assign start position
        interceptor[interceptorNumber].origin =   {x=launcher[launcherShooting].position.x, y = launcher[launcherShooting].position.y};
        interceptor[interceptorNumber].position = {x=interceptor[interceptorNumber].origin.x, y =interceptor[interceptorNumber].origin.y};
        interceptor[interceptorNumber].objective = {x= cursorPos.x, y = cursorPos.y};

        --// Calculate speed
        module = math.sqrt(
                (interceptor[interceptorNumber].objective.x - interceptor[interceptorNumber].origin.x) ^ 2 +
                (interceptor[interceptorNumber].objective.y - interceptor[interceptorNumber].origin.y) ^ 2
        );

        sideX = (interceptor[interceptorNumber].objective.x - interceptor[interceptorNumber].origin.x) * INTERCEPTOR_SPEED / module;
        sideY = (interceptor[interceptorNumber].objective.y - interceptor[interceptorNumber].origin.y) * INTERCEPTOR_SPEED / module;

        interceptor[interceptorNumber].speed = {x= sideX, y=sideY };

        --// Update
        interceptorNumber = interceptorNumber + 1;
        if (interceptorNumber == MAX_INTERCEPTORS) then 
            interceptorNumber = 1 
        end;


        fire1 = false;fire2 =false; fire3 = false;
    end
end
function UpdateIncomingFire()

    --missileIndex = 1;

    --// Launch missile
    if (framesCounter % MISSILE_LAUNCH_FRAMES == 0)
    then
        --// Activate the missile
        missile[missileIndex].active = true;

        --// Assign start position
        missile[missileIndex].origin = { x = math.random(20, Raylib.GetScreenWidth() - 20), y = -10 };
        missile[missileIndex].position.x = missile[missileIndex].origin.x;
        missile[missileIndex].position.y = missile[missileIndex].origin.y;
        missile[missileIndex].objective = {x= math.random(20, Raylib.GetScreenWidth() - 20),y = Raylib.GetScreenHeight() + 10 };

        --// Calculate speed
        local module = math.sqrt(
            (missile[missileIndex].objective.x - missile[missileIndex].origin.x) ^ 2 +
            (missile[missileIndex].objective.y - missile[missileIndex].origin.y) ^ 2
        );

        local sideX = (missile[missileIndex].objective.x - missile[missileIndex].origin.x) * MISSILE_SPEED / module;
        local sideY = (missile[missileIndex].objective.y - missile[missileIndex].origin.y) * MISSILE_SPEED / module;

        missile[missileIndex].speed = {x = sideX, y= sideY };

        --// Update
        missileIndex = missileIndex+1;
        if (missileIndex == MAX_MISSILES) then 
            missileIndex = 1 
        end

    end
end


function drawCursor()
	cursorPos.x, cursorPos.y = Raylib.GetMousePosition();
    local cursor_rad = 16;
    local cursor_color = 0xff00003e;
    Raylib.DrawCircle(cursorPos.x, cursorPos.y, cursor_rad, cursor_color)
end
