
function onSceneStart()
	createHub();
end

function onSceneEnd() 

end
function Update(dt) 

end

function Draw() 
end

function UIDraw() 
	Raylib.DrawTextEx("arial", "Mission Giving Course", 25, Raylib.GetScreenHeight()-40, 24,1.0, 0xffffffff)
end

function onKeyPress(key)
	listenForPortalActivation(key)
	
	if(TaskMaster.do_generate) then
		GenerateTask( {x1 = -30, x2 = -10, y1 = -10, y2 =1}, { obj = platforms[math.random(1, #platforms)], xoff = 0, yoff = -1} );
	end
end

function onBeginContact(A, B)
	handleLetterDelivery(A,B);
end


function onEndContact(A, B)

end

--/ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

function createHub()
	TextureManager.Add("Assets/Textures/dummy", "dummy");
	TextureManager.Add("Assets/Textures/kenny/inputs", "inputs")
	-- // create ground
		ground = Scene.CreateNode2d("ground");
		ground.transform.position:set(0, 1);
		ground.transform.size:set(100, 0.1);
		ground.transform:Center();
		ground.material:SetColor(0x00ff00ff)
		ground.rigidbody.bdyDef.type = 0
		ground.rigidbody.fixDef.density = 1.0;
		ground.rigidbody:SetBody(Scene.GetWorld(), ground.transform, 0)

	-- //create platforms
	local origin = -15
	platforms = {
		{name = "center platform", x = 0 + origin, y = -1,  w = 5, h = 0.1, color = 0x00ff00ff},
		{name = "left platform", x = -5 + origin, y = -1.5, w = 5, h = 0.1, color = 0x00ff00ff},
		{name = "right platform", x = 5 + origin, y = -2, w = 5, h = 0.1, color = 0x00ff00ff},
	}
	for k, v in ipairs(platforms) do
		e = Scene.CreateNode2d(v.name);
			e.transform.position:set(v.x,v.y);
			e.transform.size:set(v.w, v.h);
			e.transform:Center();
			e.material:SetColor(v.color)
			e.rigidbody.bdyDef.type = 0
			e.rigidbody.fixDef.density = 1.0;
			e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)

	end

	--// create player

	mPlayer = Scene.CreatePlayerController("Player Controller", "dummy");
	mPlayer.textureScale:set(4, 2)
	mPlayer:setPosition(-27, 0)

	--// create characters
	genCharacters();
	--//create entrance portal and exit portal
	genEntranceAndExit()

	--//create camera controllers
	genCameraControllers();

	--//set default zoom
	App.GetCamera().zoom = 20.0;
end

function genEntranceAndExit()
	local icon ={texture="inputs", frame = {x=323, y= 170, w= 16,h= 16}}
	Portals = {
		{Name = "entrance portal", x = -30, y = -4, w = 3, h = 9 }
		,{Name = "exit portal", x = 30, y = -4, w = 3, h = 9 , destination = "obstacleCourse"}
	}

	for k, v in ipairs(Portals) do
	
		local e = Scene.CreateInteractableNode(v.Name);
			e.transform.position:set(v.x, v.y);
			e.transform.size:set(v.w, v.h)
			e.transform:Center();
			e.rigidbody.bdyDef.type = 1;
			e.rigidbody.fixDef.isSensor = true;
			e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)
			e:setIconDestination(0, 0, 2.0, 2.0)
			e:setIconTexture(icon.texture)
			e:setIconFrame(icon.frame.x, icon.frame.y, icon.frame.w, icon.frame.h)
			e.visible = false;
			e:setObserver(mPlayer);

			local function TransitionToScene()
				App.GoToScene(v.destination or "MissionGivingCourse")
				
			end
			v.node = e;
			v.Activation_key = 69; -- KEY_E
			v.onBeginContact = TransitionToScene;

	end
end

function listenForPortalActivation(key)
	for k, v in ipairs(Portals) do
		if v.node.isInteractive and key == v.Activation_key then
			v.onBeginContact(v);
		end
	end

end


function genCameraControllers( )

	camControllers = 
	{
	--	   {pos = {x = 0, y = -2}, size = {x=80, y=6}, onEnter = 45, onExit = 20, name  = "Street Cam"}
	}

	for k, v in ipairs(camControllers) do
		local e  = Scene.CreateCameraController2d(v.name);
		e.transform.position:set(v.pos.x, v.pos.y);
		e.transform.size:set(v.size.x, v.size.y);
		e.transform:Center();
		e.material:SetColorVec(1,1,1,0.5)
		e:SetTarget(mPlayer);
		e:Standardize();
		e.onEnter.zoom = v.onEnter;
		e.onExit.zoom = v.onExit;
		e.visible = false;
		e:SetTarget(mPlayer);
		v.node = e;

	end
end

function genCharacters()
	local icon = {alias = "inputs", frame = {x=323, y= 170, w= 16,h= 16}};
	-- Gen Task Master
	local e = Scene.CreateNPCNode("Task Master", "dummy", icon.alias, "no script");

		e.transform.position:set(-25, -3);
		e.transform.size:set(0.5, 1);
		e.transform:Center();
		e.textureScale:set(4, 2)
		e.rigidbody.bdyDef.type = 2;
		e.rigidbody.bdyDef.fixedRotation = true;
		e.rigidbody.fixDef.density = 1.0;
		e.rigidbody.fixDef.friction = 1.0;
		e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)
		e.icon.frame:set(icon.frame.x,icon.frame.y,icon.frame.w, icon.frame.h)
		e.text:setText("Here.\nHave a Task!", true)
		e.text.fontSize = 32
		e.text:SetFont("comic")
		b2d.AddCircleSensor(e.rigidbody, 1.25)
		e.prompter = mPlayer;

	TaskMaster = 
	{
		node = e,
		id = e:GetID(),
		task_given = false,
		task_completed = false,
		not_done_strings = {
			"Finish your current task, then I'll give you another.", 
			"My my, You are eager for work eh?\nStart by on Finishing your #your_current_task task, then come and talk to me again.", 
			"Still on your #your_current_task task?\nHow long until you wrap that up huh?"},
		completion_strings = {
			"Bravo!!\nThe reward for hard work is...\n you guessed it! More work.",
			"Why thank you for the quick turnaround!\nHere's another job, as a treat..",
			"Ahh now that you have finished your #your_current_task delivery\nHere is another task for you!"
		}
	}

	math.randomseed(os.time());
end




-- Task 

function ordinal_numbers(n)
	  local ordinal, digit = {"st", "nd", "rd"}, string.sub(n, -1)
		  if tonumber(digit) > 0 and tonumber(digit) <= 3 and string.sub(n,-2) ~= 11 and string.sub(n,-2) ~= 12 and string.sub(n,-2) ~= 13 then
			return n .. ordinal[tonumber(digit)]
		  else
			return n .. "th"
	  end
end

function onDialogueStart(character, player ) 
	if character:GetID() == TaskMaster.id and TaskMaster.task_completed then
		TaskMaster.task_given = false;
		TaskMaster.task_completed = false;
		local nextString = TaskMaster.completion_strings[math.random(1, #TaskMaster.completion_strings)];
		
		local replacementString = ordinal_numbers(TaskManager.task_counter);
		nextString = string.gsub(nextString, "#your_current_task", replacementString)

		character.text:setText(nextString, true);
	end
end
function onDialogueEnd(character, player ) 
	-- the only npc is the task master, here but....[but what..?]
	if character:GetID() == TaskMaster.id then
		if(not TaskMaster.task_given) then 
			TaskMaster.task_given = true;
			character.text:setText(TaskMaster.not_done_strings[1], true);
			TaskMaster.do_generate = true;
		else
			local nextString = TaskMaster.not_done_strings[math.random(1, #TaskMaster.not_done_strings)];

			local i, j = string.find(nextString, "#your_current_task")
			local replacementString = ordinal_numbers(TaskManager.task_counter);
			nextString = string.gsub(nextString, "#your_current_task", replacementString)
			character.text:setText(nextString, true);
		end

		print("Dialogue Ended with character: ", character.Name)
	end
end


TaskManager = {
	mail = nil,
	mail_box = nil,
	in_proggress = false,
	task_counter = 0,
	Activation_key = 69
};


function GenerateTask(spawnBounds, goalBounds)
	TaskMaster.do_generate = false;

	local x = math.random(spawnBounds.x1, spawnBounds.x2);
	local y = math.random(spawnBounds.y1, spawnBounds.y2);
	local w =1-- math.random(1, 8);
	local h =1-- math.random(1, w);

	TaskManager.task_counter = TaskManager.task_counter + 1;
	task_name = "Mail-"..TaskManager.task_counter;

	local r = math.random();
	local g = math.random();
	local b = math.random();

	local e = Scene.CreateNode2d(task_name);
		e.transform.position:set(x, y);
		e.transform.size:set(w, h);
		e.transform:Center();
		e.material:SetColorVec(r, g, b, 1.0)
		e.rigidbody.bdyDef.type = 2
		e.rigidbody.fixDef.density = 1--math.random(1, 10);
		e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)


	TaskManager.mail = e;

	mail_box_name = "Mailbox-"..TaskManager.task_counter;
	local icon = {alias = "inputs", frame = {x=323, y= 170, w= 16,h= 16}};

	x = goalBounds.obj.x + goalBounds.xoff
	y = goalBounds.obj.y + goalBounds.yoff
	w = goalBounds.obj.w
	h = 1

	local i = Scene.CreateInteractableNode(mail_box_name);
		i.transform.position:set(x, y );
		i.transform.size:set(w,h)
		i.transform:Center();
		i.rigidbody.bdyDef.type = 1;
		i.rigidbody.fixDef.isSensor = true;
		i.rigidbody:SetBody(Scene.GetWorld(), i.transform, 0)
		i.material:SetColorVec(r, g, b, 0.55)
		i:setIconDestination(0, 0, 0.0, 0.0)
		i:setIconTexture(icon.alias)
		i:setIconFrame(icon.frame.x, icon.frame.y, icon.frame.w, icon.frame.h)
		i.visible = true;
		i:setObserver(e);

	TaskManager.mail_box = i;

	TaskManager.in_proggress = true;

	print("Generated task ", TaskManager.task_counter, TaskManager.in_proggress);
end

function handleLetterDelivery(A, B)
	if TaskManager.in_proggress and TaskManager.mail_box.isInteractive then
		TaskManager.mail.alive = false;
		TaskManager.mail_box.alive = false;
		TaskManager.in_proggress = false;
		TaskMaster.task_completed = true;
	end
end
