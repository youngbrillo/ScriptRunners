
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

	if(gameOver == false) then
		DrawInProg()
	end

	DrawInstructions();
end


function onKeyPress(key)
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
	MISSILE_LAUNCH_FRAMES       =80
	INTERCEPTOR_SPEED           =10
	EXPLOSION_INCREASE_TIME     =90          --// In frames
	EXPLOSION_TOTAL_TIME        =210         --// In frames

	EXPLOSION_COLOR              =0x7D7D7D7D;
		
	startPhase();
	math.randomseed(os.time());

end

function startPhase()

    --// Initialize missiles
	for i = 1, MAX_MISSILES, 1 do
		missile[i] = {
			origin = {x = 0, y = 0},
			objective = {x = 0, y = 0},
			speed = {0, 0},
			position = {0, 0},
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
            Raylib.DrawLine(int.origin.x, int.origin.y, int.position.x, int.position.y, 0x00ff00ff);

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
    for i, launch in ipairs(launcher) do
        if (launch.active) then
			Raylib.DrawRectangle(launch.position.x - LAUNCHER_SIZE/2, launch.position.y - LAUNCHER_SIZE/2, LAUNCHER_SIZE, LAUNCHER_SIZE, 0x5d5d5dff);
		end
	end

    for i, build in ipairs(building) 
    do
        if (build.active)  then
			Raylib.DrawRectangle(build.position.x - BUILDING_SIZE/2, build.position.y - BUILDING_SIZE/2, BUILDING_SIZE, BUILDING_SIZE, 0xdededeff);
		end
    end

end


function DrawInstructions()

    Raylib.DrawText("SCORE ".. score, 20, 20, 40, 0xC87DC8FF);

	--local vx, vy = Raylib.MeasureText(inText, 20)
	--Raylib.DrawText(inText, Raylib.GetScreenWidth() - (vx + 10), Raylib.GetScreenHeight() - (vy + 20), 20, 0xffffffff)

end


function HandleGameRules(dt)
	if(gameOver == false) then
	    framesCounter = framesCounter + 1;
		update_Interceptors(dt);
		update_Missiles(dt);
		update_Explosions(dt);

		UpdateOutgoingFire();
		UpdateIncomingFire();
	end
end

function update_Interceptors(delta)

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
            
            handleBuildingCollisions();
            handleLauncherCollisions();
            handleExplosionCollisions();

            end
        end
	end

end

function update_Explosions(delta)
	
end

function handleLauncherCollisions()
    --// CHeck collision with launchers
    --[[
    for (int j = 0; j < LAUNCHERS_AMOUNT; j++)
    do
        if (launcher[j].active)
        then
            if (Raylib.CheckCollisionPointRec(
                missile[i].position, 
                launcher[j].position.x - LAUNCHER_SIZE / 2, launcher[j].position.y - LAUNCHER_SIZE / 2,
                LAUNCHER_SIZE, LAUNCHER_SIZE
            }))
            then
                --// Missile dissapears
                missile[i].active = false;

                --// Explosion and destroy building
                launcher[j].active = false;

                explosion[explosionIndex].position = missile[i].position;
                explosion[explosionIndex].active = true;
                explosion[explosionIndex].frame = 0;
                explosionIndex = explosionIndex+1;
                if (explosionIndex == MAX_EXPLOSIONS) then explosionIndex = 0 end

                break;
            end
        end
    end
    ]]

end

function handleBuildingCollisions()
    --// CHeck collision with buildings
    --[[
    for (int j = 0; j < BUILDINGS_AMOUNT; j++)
    then
        if (building[j].active)
        then
            if (Raylib.CheckCollisionPointRec(missile[i].position, Rectangle {
                building[j].position.x - BUILDING_SIZE / 2, building[j].position.y - BUILDING_SIZE / 2,
                    BUILDING_SIZE, BUILDING_SIZE
            }))
            then
                // Missile dissapears
                missile[i].active = false;

                // Explosion and destroy building
                building[j].active = false;

                explosion[explosionIndex].position = missile[i].position;
                explosion[explosionIndex].active = true;
                explosion[explosionIndex].frame = 0;
                explosionIndex++;
                if (explosionIndex == MAX_EXPLOSIONS) then explosionIndex = 0; end

                break;
            end
        end
    end
    ]]
end
function handleExplosionCollisions()
    --// CHeck collision with explosions
    --[[
    for (int j = 0; j < MAX_EXPLOSIONS; j++)
    then
        if (explosion[j].active)
        then
            if (CheckCollisionPointCircle(missile[i].position, explosion[j].position, EXPLOSION_RADIUS * explosion[j].radiusMultiplier))
            then
                --// Missile dissapears and we earn 100 points
                missile[i].active = false;
                score += 100;

                explosion[explosionIndex].position = missile[i].position;
                explosion[explosionIndex].active = true;
                explosion[explosionIndex].frame = 0;
                explosionIndex++;
                if (explosionIndex == MAX_EXPLOSIONS) explosionIndex = 0;

                break;
            end
        end
    end
    ]]
end
function UpdateOutgoingFire()

end
function UpdateIncomingFire()

    missileIndex = 1;

    --// Launch missile
    if (framesCounter % MISSILE_LAUNCH_FRAMES == 0)
    then

        --// Activate the missile
        missile[missileIndex].active = true;

        --// Assign start position
        missile[missileIndex].origin = { x = math.random(20, Raylib.GetScreenWidth() - 20), y = -10 };
        missile[missileIndex].position = missile[missileIndex].origin;
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
        if (missileIndex == MAX_MISSILES) then missileIndex = 1 end
    end
end
