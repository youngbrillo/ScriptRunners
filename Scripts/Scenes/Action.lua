-- hooked functions -- /////////////////////////////////////////////////////////////////////////////////////
function onSceneStart()
	CreateBuildings();
end

function onSceneEnd() 

end

function Update(dt) 
	UpdateHazards(dt);
end

function Draw() 

end

function UIDraw() 

end

function onKeyPress(key)
	if(key == 258) then --//tab key
		App.GoToScene("Action")
	end
end
-- my functions -- /////////////////////////////////////////////////////////////////////////////////////

WHITE		= 0xFFFFFFFF
RED			= 0xFF0000FF
GREEN		= 0x00FF00FF
BLUE		= 0x0000FFFF
LIGHTGRAY	= 0xC8C8C8FF
GRAY		= 0x828282FF
SHADOW		= 0x000000FF

function CreateBuildings()
	GBuildings = {};
	
	local m_structures = {
		--  {name = "ground", x = 0, y = 0, w = 100, h = .1, color = GREEN}
		--, {name = "building a", x = 0, y = 0, w = 3, h = 10, color = LIGHTGRAY}
		--, {name = "building b", x = -4, y = 0, w = 3, h = 10, color = LIGHTGRAY}
		--, {name = "building c", x = 4, y = 0, w = 3, h = 10, color = LIGHTGRAY}
	}
	math.randomseed(os.time());
	local startX = -50
	for i = 1, 30 do
		local xPos		= startX
		local width		= 3;
		local height	= math.random(4, 10);
		local spacing   = 2;
		local yPos		= -height /2
		local _Color = GRAY;
		if( i % 2 == 0) then _Color = SHADOW end
		local building = {
			name = "building " .. (i - 1),
			x = xPos,	
			y = yPos,
			w = width,
			h = height,
			color = _Color
		}	
		startX = startX + width/2 + spacing
		m_structures[i] = building;
	end



	local ground =   Scene.CreateNode2d("ground");
		ground.transform.position:set(-75.0 , 0.0)
		ground.transform.size:set(75.0, 0.0);
		ground.material:SetColor(GREEN);
		ground.material.shape = 2;

	for k, v in ipairs(m_structures) do
		local e = Scene.CreateNode2d(v.name);
		e.transform.position:set(v.x, v.y)
		e.transform.size:set(v.w, v.h);
		e.transform:Center();
		e.material:SetColor(v.color);
		e.material:SetColorVec(math.random() , math.random(), math.random(), 0.74)
		if v.rigidbody ~= nil then
			printf("No rigidbody settings just yet")
		end

		GBuildings[k] = e;

	end

	App.GetCamera().zoom = 8.5;
	App.GetCamera().offset.y = Raylib.GetScreenHeight() * 0.9;

	print("Total structures: " .. #GBuildings)

	GHazard_container = {}
	GHazard_count = 1;
	GH_settings = { --//global Hazard settings
		total = 20,
		min_speed = 1,
		max_speed = 85,
		xBounds = {min = -50, max = 50},
		yBounds = {min =  -100, max = -80},
		timer = {target = 1.0, progress = 0.0},
		destroy_depth = 4
	}
end


function UpdateHazards(dt)
	GH_settings.timer.progress = GH_settings.timer.progress + dt;
	if GH_settings.timer.progress >= GH_settings.timer.target then
		GH_settings.timer.progress = 0.0;
		GenerateHazards(dt);
	end
	for k, v in ipairs(GHazard_container) 
	do
		if(v.transform.position.y > GH_settings.destroy_depth) 
		then
			--print("->", k, v, GH_settings.destroy_depth, v.transform.position.y)
			v.alive = false;
			--GHazard_container[k] = nil;
			table.remove(GHazard_container, k);
		end
	end
end


function GenerateHazards(dt)
	if #GHazard_container < GH_settings.total then
		-- generate new hazard item
		local name = "Hazard "..GHazard_count;
		local e = Scene.CreateNode2d(name);
		local v = {
			x = math.random(GH_settings.xBounds.min, GH_settings.xBounds.max),
			y = math.random(GH_settings.yBounds.min, GH_settings.yBounds.max),
			w = 1,
			h = 1
		}
		e.transform.position:set(v.x, v.y)
		e.transform.size:set(v.w, v.h);
		e.transform:Center();
		e.material:SetColorVec(0.0, 0.0, 0.0, 0.74)
		e.material:SetColor(0xFFFFFFFF);
		if v.rigidbody ~= nil then
			print("No rigidbody settings just yet")
		end
		local objective = {
			x = math.random(GH_settings.xBounds.min, GH_settings.xBounds.max), 
			y = - v.y
		}
		e.direction:set(objective.x, objective.y);
		e.speed = math.random(1, 2);

		table.insert(GHazard_container,  e);
		--print("HAZARD CONTAINER", #GHazard_container)
	end
end

