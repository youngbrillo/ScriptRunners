

function onSceneStart()
	-- Add default textures
	TextureManager.Add("Assets/Textures/dummy", "dummy");
	TextureManager.Add("Assets/Textures/kenny/inputs", "inputs")
	-- initialize variables
	save_file = File.AppSave("./configs/autoSave.json");
	mPlayer = nil
	npcs = {};

	-- initialize environment and objects

	CreateBackGrounds();	-- add back ground pngs
	createTown();			-- add town structures
	createTasks();			-- add objectives
	createCamControllers();	-- add cam controllers
	loadSaveData();			-- load other save Data
	
	App.GetCamera().zoom = 25.0;
	math.randomseed(os.time());
end



function onSceneEnd() 
	RemoveBackGroundTextures();
	uploadSaveData();
end
function Update(dt) 

end

function Draw() 

end
function UIDraw() 
	Raylib.DrawTextEx("arial", "Town Hub", 25, Raylib.GetScreenHeight()-60, 40 ,1.0, 0xffffffff)
end

spawn_player_key = 81; --//Q
function onKeyPress(key)
	--if key == spawn_player_key then SpawnPlayer(0, 12); end
	listenForSwitchStateChange(key);
end
function onBeginContact(A, B)

end
function onEndContact(A, B)

end

-- ///////////////////////////////////////////////////////////////////////////////////////////
function loadSaveData()
	if (save_file ~= nil) then
		SpawnPlayer(
			save_file:GetParameterfloat("player_position_x"),
			save_file:GetParameterfloat("player_position_y")
			);
	end
end
function uploadSaveData()
	if save_file ~= nil then
		save_file:AddParameter_string("current_location", "town.lua")
		if mPlayer ~= nil then
			save_file:AddParameter_float("player_position_x", mPlayer.transform.position.x)
			save_file:AddParameter_float("player_position_y", mPlayer.transform.position.y)
		end
		save_file:SaveFile("./configs/autoSave.json");
	end
end

function SpawnPlayer(x, y )
	if(mPlayer ~= nil) then
		mPlayer.alive = false;
	end

	mPlayer = Scene.CreatePlayerController("Player Controller", "dummy");
	--mPlayer = Scene.CreatePlayerController2("Player Controller", "dummy", "Player.ini");
	mPlayer.textureScale:set(4, 2)
	mPlayer:setPosition(x,y)

	for k, v in ipairs(npcs) do
		v.node.prompter = mPlayer;
	end

	for k, v in ipairs(interactables) do
		v.node:setObserver(mPlayer);
	end
	

	for k, v in ipairs(camControllers) do
		v.node:SetTarget(mPlayer);
		v.node:Standardize();
	end
end

--/ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function CreateBackGrounds() 
	bgTextures = {};
	local base_path = "Assets/Textures/";
	local folder_path = "clouds_craft-pixel/Clouds/Clouds 1/"
		table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."1", "bg_1"), name = "bg_1", uv_x = 2});
		table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."2", "bg_2"), name = "bg_2", uv_x = 4});
		table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."3", "bg_3"), name = "bg_3", uv_x = 6});
		table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."4", "bg_4"), name = "bg_4", uv_x = 8});

	
	local cityNumber = "01"
	folder_path = "DigitalMoons/Pixelcity"..cityNumber.."/"
		--table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."Pixelcity"..cityNumber.."_layer" .. "01", "cityLayer1"), name = "cityLayer1", uv_x = 2});
		--table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."Pixelcity"..cityNumber.."_layer" .. "02", "cityLayer2"), name = "cityLayer2", uv_x = 4});
		--table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."Pixelcity"..cityNumber.."_layer" .. "03", "cityLayer3"), name = "cityLayer3", uv_x = 5});
		--table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."Pixelcity"..cityNumber.."_layer" .. "04", "cityLayer4"), name = "cityLayer4", uv_x = 6});
		--table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."Pixelcity"..cityNumber.."_layer" .. "05", "cityLayer5"), name = "cityLayer5", uv_x = 0});
		--table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."Pixelcity"..cityNumber.."_layer" .. "06", "cityLayer6"), name = "cityLayer6", uv_x = 8});

	--table.insert(bgTextures, {id = TextureManager.Add(base_path.."city/street", "m_city_street"), name = "m_city_street", uv_x = 0, x = 0, y = -4.5, w = 100, h = 50});

	for k, v in ipairs(bgTextures)
	do
		local p = Scene.CreateNode2d("Background-"..v.name);
		p.transform.position.y = v.y or 0
		p.transform.size.x = v.w or 100;
		p.transform.size.y = v.h or 40;
		p.transform:Center();
		p.material:SetTextureByAlias(v.name)
		p.material.uv_scroll:set(v.uv_x, 0)
	end
end

function RemoveBackGroundTextures()
	for k, v in ipairs(bgTextures)
	do
		TextureManager.Remove(v.id);
	end
end
--/ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


function createTown()
	map = Scene.CreateTilemapNode("tile map")
	map:LoadData("./Assets/Textures/city/Town.json");
	CreateBuildings();
	--//generate characters
	AddNPCs();
	CreateBuildingInteriors();
	CreateInteractableObjects(map);
	-- add package dispenser
	--package_manager = Scene.CreateNode2d("Package Manager"); package_manager.transform.position:set(-20.5, 6.5); package_manager.material:SetColor(0xffff007a);

end

function CreateBuildings()
	-- simple blockout for where the buildings will go
	local interior_color = 0x5E5E5EFF
	building_blockout = {
		{x = -37, y = 11.0, w = 6.0, h = 10.0},
		{x = -37, y = 14, w = 6, h = 4.0, c = interior_color , is_interior = true, iw = 3, ih = 0.45, ic = 0xa8a8a8ff },
		{x = -18, y = 11.0, w = 6.0, h = 10.0 },
		{x = -18, y = 9, w = 6, h = 4.0, c = interior_color , is_interior = true, ic = 0xa8a8a8ff, ih = 0.4, iw=5 },
		{x = -18, y = 14, w = 6, h = 4.0, c = interior_color, is_interior = true, ic = 0xa8a8a8ff, ih = 0.4, iw=5},
		{x = -11, y = 6.0, w = 8.0, h = 20.0 },
		{x = -11, y = -1, w = 8, h = 4.0, c = interior_color  , is_interior = true, ic = 0xa8a8a8ff,ih = 0.33, iw=3},
		{x = 28, y = 11.0, w = 35, h = 10.0, c = 0x5E5E5EFF  },
		{name = "apt 1", x = 13, y = 12.50, w = 6, h = 7.0} ,-- c = 0xA8A8A878}, 
		{name = "apt 2", x = 21, y = 8.5, w = 6, h = 15.0 },
		{name = "apt 3", x = 30.5, y = 10.50, w = 9, h = 11.0 }, 
		{name = "apt 4", x = 40, y = 1.50, w = 6, h = 29.0 },
		{name = "wall", x = 45, y = 11, w = 1, h = 10.0 }
	}
	for k, v in ipairs(building_blockout) do
		local e = Scene.CreateNode2d(v.name or "Building::"..k)
		e.transform.position:set(v.x, v.y);
		e.transform.size:set(v.w, v.h);
		e.material:SetColor(v.c or 0xa8a8a8ff)
		e.transform:Center();
		
		v.node = e;
	end
end
function CreateBuildingInteriors()
	for k, v in ipairs(building_blockout) do
		if v.is_interior then
			local e = Scene.CreateNode2d("Building_interor::"..k);
			local height = v.ih or 0.5;
			e.transform.position:set(v.x, v.y + v.h/2 - height/2);
			e.transform.size:set(v.iw or v.w, height);
			e.material:SetColor(v.ic or 0x000000ff);
			e.transform:Center();
			v.interior_node = e;
		end
	end
end

function CreateInteractableObjects(anchor)

	interactables = {};

	elevator_persistence = save_file:GetParameterBool("elevator_is_up")

	CreatePullyObject(anchor, 
		{ name = "elevator", x = -14, y = 6, w = 2, h = 0.5},
		{	x = 0, y = 1, 
			motorSpeed = -3.0, maxMotorForce = 1000.0, enableMotor = elevator_persistence, enableLimit = true, 
			lowerTranslation = -4.75, upperTranslation = 9.75
		},
		{
			{name="switch on", x = -12.75, y = 15.5, w = .5, h = 1, color = 0xffffffa7, 
				func = function(joint)
					if joint:IsMotorEnabled() then
						joint:EnableMotor(false);
						save_file:AddParameter_bool("elevator_is_up", false)
					else
						joint:EnableMotor(true);
						save_file:AddParameter_bool("elevator_is_up", true)
					end
				end
			},
			{name="switch off", x = -15.25, y = .5, w = .5, h = 1, color = 0x000000a7,
				func = function(joint)
					if joint:IsMotorEnabled() then
						joint:EnableMotor(false);
						save_file:AddParameter_bool("elevator_is_up", false)
					else
						joint:EnableMotor(true);
						save_file:AddParameter_bool("elevator_is_up", true)
					end
				end
			},
		}
	);

	--[[
		local e = Scene.CreateNode2d("test")
		e.transform.position:set(-14, 10);
		e.transform.size:set(1, 1);
		e.material:SetColor(0xa8a833ff)
		e.transform:Center();
	]]
end

function CreatePullyObject(anchor, v, prismaticDef, switches)
	local e = Scene.CreateNode2d(v.name);

	e.transform.position:set(v.x, v.y);
	e.transform.size:set(v.w, v.h);
	e.transform:Center();
	e.material:SetColor(v.color or 0x000000ff);
	e.rigidbody.bdyDef.type = 2
	e.rigidbody.fixDef.density = 1.0;
	e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)
	--joint creation
	v.joint = b2d.CreatePrismaticJoint(Scene.GetWorld(), anchor, e, prismaticDef);

	for k, s in ipairs(switches) do
		local d = CreatePullyControl(e, v.joint, s);
	end
end

function CreatePullyControl(observer, joint, switch) 
	local icon ={texture="inputs", frame = {x=323, y= 170, w= 16,h= 16}}
	local e = Scene.CreateInteractableNode(observer.Name.."_"..switch.name);
		e.transform.position:set(switch.x, switch.y);
		e.transform.size:set(switch.w, switch.h)
		e.transform:Center();
		e.material:SetColor(switch.color or 0xffff00ff);
		e.rigidbody.bdyDef.type = 1;
		e.rigidbody.fixDef.isSensor = true;
		e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)
		e:setIconDestination(0, -(e.transform.size.y + 0.5), 1.0, 1.0)
		e:setIconTexture(icon.texture)
		e:setIconFrame(icon.frame.x, icon.frame.y, icon.frame.w, icon.frame.h)

		switch.Activation_key = 69; -- KEY_E
		switch.joint = joint;
		switch.node = e;

	table.insert(interactables, switch)
end

function listenForSwitchStateChange(key)
	for k, v in ipairs(interactables) do
		if v.node.isInteractive and key == v.Activation_key then
			v.func(v.joint);
		end
	end
end
function createCamControllers()
	camControllers = 
	{
		{x = -27.5, y = 14, w=25, h=4, onEnter = 34, onExit = 18},
		{x = -11, y = 6, w=8, h=17, onEnter = 34, onExit = 18},
		--{x = 0, y = 0, w=1, h=1, onEnter = 34, onExit = 18},
	}

	for k, v in ipairs(camControllers) do
		local e  = Scene.CreateCameraController2d(v.name or "camera-controller-"..k);
		e.transform.position:set(v.x, v.y);
		e.transform.size:set(v.w, v.h);
		e.transform:Center();
		e.material:SetColorVec(1,1,1,0.5)
		--e:SetTarget(mPlayer);
		e:Standardize();
		e.onEnter.zoom = v.onEnter;
		e.onExit.zoom = v.onExit;
		e.visible = false;
		v.node = e;
	end
end

function AddNPCs()
	local icon = {alias = "inputs", frame = {x=323, y= 170, w= 16,h= 16}};
	npcs = {
		{name = "Vize", text = "I've got some work for ya.\nGo talk to Boss", font = "comic", x = -37, y = 12, handleDialogueStart = TalkStart_SuperVizer},
		{name = "Boss", text = "Whatcha Talking to me for go see Vize'!", font = "comic", x = -18, y = 13,  handleDialogueStart = TalkStart_Boss, tIndex = 1},
		{name = "Navi", text = "Ahem! If you need directions look no further!", font = "comic", x = -18, y = 9,  handleDialogueStart = TalkStart_Navi, instructionProg = 1, dispensed_package = false},
		{name = "Elevator Man", handleDialogueStart = TalkStart_ElevatorMan, iCount = 1, isYearningExpressed = false, x = -11,  y = 0,  text = ". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n. . . I wonder if Navi is upset with me. . .", font = "comic"},
	}
	-- Gen Task Master
	for k, v in ipairs(npcs) do
		local e = Scene.CreateNPCNode(v.name, "dummy", icon.alias, "npc.script.lua");
			e.transform.position:set(v.x, v.y);
			e.transform.size:set(0.5, 1);
			e.transform:Center();
			e.textureScale:set(4, 2)
			e.rigidbody.bdyDef.type = 2;
			e.rigidbody.bdyDef.fixedRotation = true;
			e.rigidbody.fixDef.density = 1.0;
			e.rigidbody.fixDef.friction = 1.0;
			e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)
			e.icon.frame:set(icon.frame.x,icon.frame.y,icon.frame.w, icon.frame.h)
			e.text:setText(v.text)
			e.text.fontSize = 32
			e.text:SetFont(v.font)
			e.prompter = mPlayer;
			e.material.direction.x = -1;
			b2d.AddCircleSensor(e.rigidbody, 1.25)
			v.node = e;
			v.id = e:GetID();

			v.handleDialogueStart = v.handleDialogueStart or function()  end
	end
end
function createTasks()
	local bounds = { min = {x = -10, y = -10}, max = {x = 10, y = 10}};
	jobs = {}
	jobs["unlock_merchant"] = {
		title = "Unlock Merchant", 
		Description = "Go Talk to the boss", 
		mapVal = "unlock_merchant_assigned", 
		reset = true,
		questLine = quest_line_unlock_merchant,
		flags = {
			talked_to_quest_giver = false, 
			talked_to_boss = false, 
			got_package = false, 
			lost_package = false,
			delivered	= false
		}
	};
	JobManager = Scene.CreateTaskManagerNode("job manager");
	for k, v in pairs(jobs) do -- use pairs instead of ipairs to iterate over non integer fields
		v.job_id = JobManager:CreateDeliveryTask(v.title, v.Description);
		if(v.reset and v.mapVal) then
			save_file:AddParameter_bool(v.mapVal, false);
		end
		if(save_file:GetParameterBool(v.mapVal)) then 
			JobManager.currentObjective = v.job_id; 
		end
		print("Job '".. k .. "' id is: "..v.job_id)
	end
end
function TalkStart_SuperVizer(npc, node)
	-- // does the player have unlock_merchant unlocked? 
	if not save_file:GetParameterBool("unlock_merchant_assigned") then
		save_file:AddParameter_bool("unlock_merchant_assigned", true);
		-- // make the current objective the unlock merchant objective
		JobManager.currentObjective = jobs["unlock_merchant"].job_id;
	else

		local dialogue = 
		{
			"Go see the boss.",
			"Find yerself in front of the boss.\nHe's right down the hall, you don't even gotta jump!",
			"You still here?",
			"If you wanna make it here I sure hope you can follow directions better than this."
		}
		local choice = math.random(1, #dialogue);
		node.text:setText(dialogue[choice], true);
	end
end

function TalkStart_Boss (npc, node)
	local dialogue = 
	{
		"Yo, I got something for ya.",
		"I wantchu to deliver this Package to the elevator man in the building over on the Eastside",
		"Navi can tell you how to ge there.\nYou can Find him upstairs.",
	}

	if(save_file:GetParameterBool("unlock_merchant_assigned")) then
		if save_file:GetParameterBool("unlock_merchant_talkedToBoss") then
			npc.tIndex =2
			JobManager:GetDelivery(jobs["unlock_merchant"].job_id).description = "Get Boss's package from Navi."
		end

		node.text:setText(dialogue[npc.tIndex], true);
		if npc.tIndex == 2 then 
			save_file:AddParameter_bool("unlock_merchant_talkedToBoss", true) 
			
		end

		npc.tIndex = npc.tIndex + 1;

		if(npc.tIndex > #dialogue) then
			npc.tIndex = 3;
		end
	end
end

function TalkStart_Navi(npc, node)
	local instructions = 
	{
		"Oh Did Boss send you? Here's the package!",
		"Now I bet you are looking for Elevator man.",
		"Use the elevator call box next to the boss to call the lift.",
		"It'll Make the Lift come up if it's down.\nOr go down if its already up.",
		"See ya later!"
	}
	local missionStarted = save_file:GetParameterBool("unlock_merchant_assigned");
	if(missionStarted) then

		if(npc.dispensed_package == false) then
			node.text:setText(instructions[npc.instructionProg], true);
			npc.instructionProg = npc.instructionProg + 1;
			npc.dispensed_package = true;	

			generate_package();
			JobManager:GetDelivery(jobs["unlock_merchant"].job_id).description = "Deliver Package to Elevator Man"

		else
			node.text:setText(instructions[npc.instructionProg], true);
			npc.instructionProg = npc.instructionProg + 1;
			if(npc.instructionProg > #instructions) then npc.instructionProg = 2; end
		end
	end
end

function generate_package()
	local spawn_loc = {x = -20.5, y = 6.5}
	local e = Scene.CreateNode2d("Package"); 
	e.transform.position:set(spawn_loc.x, spawn_loc.y); 
	e.material:SetColor(0xffff007a);
	e.rigidbody.bdyDef.type = 2;
	e.rigidbody.fixDef.density = 1.0;
	e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0 );
end

function TalkStart_ElevatorMan(npc, node, jobNode)
	local dialouge = 
	{
		"Hey",
		"Got something for me?",
		"Can I help you?"
	}
	local d = jobNode:GetDelivery(JobManager.currentObjective).description
	
	local instructions = {
		"Bring the package and throw it in the area behind me.",
		"I think you're supposed to \""..d .. "\" or something",
	}
	local missionStarted = save_file:GetParameterBool("unlock_merchant_assigned");
	
	if missionStarted then
		node.text:setText(instructions[npc.iCount], true);
		npc.iCount = npc.iCount + 1;
		if (npc.iCount > #instructions) then
			npc.iCount = 1;
		end
	elseif not npc.isYearningExpressed then
		npc.isYearningExpressed = true;
	else
		node.text:setText(dialouge[math.random(1, #dialouge)], true);
	end
end

function onDialogueStart(npc, npc_talk_target)

	for k, v in ipairs(npcs) do
		if(v.id == npc:GetID()) then
			v.handleDialogueStart(v, npc, JobManager);
		end
	end
end

function onDialogueEnd(npc, npc_talk_target)

end


--[[
		flags = {
			hasStarted = false
			talked_to_quest_giver = false, 
			talked_to_boss = false, 
			got_package = false, 
			lost_package = false,
			delivered	= false
		}
]]
function quest_line_unlock_merchant(flags, npc)
	if flags.hasStarted then
		if flags.has_talked_to_quest_giver then end
		if flags.talked_to_boss then end
		if flags.got_package then end
		if flags.lost_package then end
		if flags.delivered then end
	else
		-- do nothing, b/c quest hasn't started..
	end
end