
function onSceneStart()
	CreateBackGrounds();
	CreateEnvironments();
	CreateObjects();
	CreateNPCS();
	
end

function onSceneEnd() 
	RemoveBackGroundTextures();
end
function Update(dt) 
end

function Draw() 
end

function UIDraw() 
	DrawInstructions();
end

function onBeginContact(A, B)
	ListenForSceneTransitionContact(A, B);
	handleInteractableContact(A, B, true);
end

function onEndContact(A, B)
	handleInteractableContact(A, B, false);

end

function onKeyPress(key)
	--print(key)
	listenForKeyPress(key);
end


function CreateBackGrounds() 
	bgTextures = {};
	local base_path = "Assets/Textures/";
	local folder_path = "clouds_craft-pixel/Clouds/Clouds 4/"
	table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."1", "bg_1"), name = "bg_1"});
	table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."2", "bg_2"), name = "bg_2"});
	table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."3", "bg_3"), name = "bg_3"});
	table.insert(bgTextures, {id = TextureManager.Add(base_path..folder_path.."4", "bg_4"), name = "bg_4"});
	for k, v in ipairs(bgTextures)
	do
		local p = Scene.CreateNode2d("Background-"..v.name);
		p.transform.size.x = 100;
		p.transform.size.y = 40;
		p.transform:Center();
		p.material:SetTextureByAlias(v.name)
		p.material.uv_scroll:set(2 * (k-1), 0)
	end
end

function RemoveBackGroundTextures()
	for k, v in ipairs(bgTextures)
	do
		TextureManager.Remove(v.id);
	end
end
function CreateEnvironments() 

	elements =  {
           {name="floor", x = 22.5,	y = 10,	w=55,  h=.5 , blocking =true, color = 0x6ca3c3ff , hasShadow = false, dynamic = false}
         , {name="floor frag.1", x = -32.375,	y = 10,	w=35.25,  h=.5 , blocking =true, color = 0x6ca3c3ff , hasShadow = false, dynamic = false}
         , {name="sub floor 1", x = -7.25,	y = 15,	w=15,  h=.5 , blocking =true, color = 0x6ca3c3ff , hasShadow = false, dynamic = false}
         , {name="sub wall.l", x = -15,	y = 12.75,	w=.5,  h=5 , blocking =true, color = 0x6ca3c3ff , hasShadow = false, dynamic = false}
         , {name="sub wall.r", x = 0.5,	y = 12.75,	w=.5,  h=5 , blocking =true, color = 0x6ca3c3ff , hasShadow = false, dynamic = false}
         , {name="float floor 1", x = 7.75,	y = 5,	w=15,  h=.5 , blocking =true, color = 0x6ca3c3ff , hasShadow = false, dynamic = false}
         , {name="top wall.r", x = 15.5,	y = 2.75,	w=.5,  h=5 , blocking =true, color = 0x6ca3c3ff , hasShadow = false, dynamic = false}
	}

	for k, v in ipairs(elements) do
		local e = Scene.CreateNode2d(v.name);
		e.transform.position:set(v.x, v.y);
		e.transform.size:set(v.w, v.h);
		e.transform:Center();
		e.material:SetColor(v.color);
		if(v.dynamic) then
			e.rigidbody.bdyDef.type = 2
			e.rigidbody.fixDef.density = v.density
		end
		e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)

		v.node = e;
		v.alive = true;
	end


	doors = {
		{	name="Large obstacle a", x = -8.325,	y = 7.225,	w=17,  h=5 , blocking =true, color = 0x6ca3c3ff , density = 10.0, 
			definition = {
				x = 1, y = 0, motorSpeed = -10.0, maxMotorForce = 10000.0, enableMotor = false, enableLimit = true,
				lowerTranslation = -15, upperTranslation = 0
			}
		},
		{	name="Large obstacle b", x = 12.75,	y =2.25,	w=4.5,  h=4.5 , blocking =true, color = 0x6ca3c3ff , density = 5.0, 
			definition = {
				x = 0, y = 1, motorSpeed = -10.0, maxMotorForce = 10000.0, enableMotor = false, enableLimit = true,
				lowerTranslation = -10, upperTranslation = 1
			}
		}
	}
	
	for k, v in ipairs(doors) do
		local e = Scene.CreateNode2d(v.name);
		e.transform.position:set(v.x, v.y);
		e.transform.size:set(v.w, v.h);
		e.transform:Center();
		e.material:SetColor(v.color);
		e.rigidbody.bdyDef.type = 2
		e.rigidbody.fixDef.density = v.density
		e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)
		v.node = e;
		v.joint = b2d.CreatePrismaticJoint(Scene.GetWorld(), elements[1].node, e, v.definition)
		v.motorEnabled = false;
	end


	App.GetCamera().zoom = 48;
	App.GetCamera().target:set(3, 8.5);
end
function CreateObjects() 
	TextureManager.Add("Assets/Textures/dummy", "dummy")
	mPlayer = Scene.CreatePlayerController("Player Controller", "dummy");
	mPlayer.textureScale:set(4, 2)
	mPlayer:setPosition(3.0, 8.5)

	--add camera controller to floor
	local camControllers = 
	{
		--{pos = {x = 5.25, y = 7.25}, size = {x=10, y=5}, onEnter = 34, onExit = 18, name  = "Floor Cam Controller"},
		--{pos = {x = -7, y = 7.25}, size = {x=20, y=5}, onEnter = 18, onExit = 18, name  = "wide view CCTV"},
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
		v.node = e;
	end
	
	
	exitListeners = 
	{
		  {pos = {x = -12.5,  y = 12.5}, size = {x = 4.5, y = 4.5}, name="Exit to Platformer I", destination = "Platforming"}
		, {pos = {x = 13,  y = 2.5}, size = {x = 4, y = 4}, name="Exit to Platformer III", destination = "Platforming III"}
	}
	
	for k, v in ipairs(exitListeners) do
		local e  = Scene.CreateNode2d(v.name);
		e.transform.position:set(v.pos.x, v.pos.y);
		e.transform.size:set(v.size.x, v.size.y);
		e.transform:Center();
		e.material:SetColorVec(0.5, 0.5, 0.5,0.5)
		e.visible = true;
		e.rigidbody.fixDef.isSensor = true;
		e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)
		v.node = e;
		v.alive = true;
	end


	--//create triggers to move the boxes
	--TextureManager.Add("Assets/Textures/buttons", "buttons")
	TextureManager.Add("Assets/Textures/kenny/inputs", "inputs")

	big_obstacle_moved = false;
	small_obstacle_moved = false;
	local function _destroy_node_7()
		big_obstacle_moved = true;
		if(doors[1].motorEnabled == false) then
			doors[1].motorEnabled = true; --not doors[1].motorEnabled;
			doors[1].joint:EnableMotor(doors[1].motorEnabled)
		else 
			doors[1].definition.motorSpeed = -1 * doors[1].definition.motorSpeed;
			doors[1].joint:SetMotorSpeed(doors[1].definition.motorSpeed);
		end
	end
	
	local function _destroy_node_8()
		small_obstacle_moved = true;
		if(doors[2].motorEnabled == false) then
			doors[2].motorEnabled = true; --not doors[1].motorEnabled;
			doors[2].joint:EnableMotor(doors[2].motorEnabled)
		else 
			doors[2].definition.motorSpeed = -1 * doors[2].definition.motorSpeed;
			doors[2].joint:SetMotorSpeed(doors[2].definition.motorSpeed);
		end
	end

	interactables = {
		{
			name="big box mover", 
			pos={x = 7, y=9.25}, 
			size={x = 1, y = 1}, 
			color = 0xffffff7E, 
			icon={texture="inputs", frame = {x=323, y= 170, w= 16,h= 16}},
			onBeginContact = _destroy_node_7,
			Activation_key = 69,
			isActivated = false
		}
		,{
			name="small box mover", 
			pos={x = 3, y=4}, 
			size={x = 1, y = 1}, 
			color = 0xffffff7E, 
			icon={texture="inputs", frame = {x=323, y= 170, w= 16,h= 16}},
			onBeginContact = _destroy_node_8,
			Activation_key = 69,
			isActivated = false
		}

	}
	for k, v in ipairs(interactables) do
		local e = Scene.CreateInteractableNode(v.name);

		e.transform.position:set(v.pos.x, v.pos.y);
		e.transform.size:set(v.size.x, v.size.y);
		e.transform:Center();
		e.material:SetColor(v.color)
		e.rigidbody.fixDef.isSensor = true;
		e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)
		e:setIconDestination(0, -(e.transform.size.y + 0.5), 1.0, 1.0)
		e:setIconTexture(v.icon.texture)
		e:setIconFrame(v.icon.frame.x, v.icon.frame.y, v.icon.frame.w, v.icon.frame.h)
		e:setObserver(mPlayer);
		v.node = e;
	end



end
function DrawInstructions() 
	Raylib.DrawText("Platformer II", 25, 25, 25, 0xffffffff)

end

function ListenForSceneTransitionContact(A, B)
	local other = nil;
	local target = nil
	for k, v in ipairs(exitListeners) do
		if(v.node.Name == A.Name) then
			target = v;
			other = B;
			break;
		elseif(v.node.Name == B.Name) then
			target = v;
			other = A;
			break;
		end
	end

	if(other ~= nil and target ~= nil) then
		if(other.Name == mPlayer.Name) then
			App.GoToScene(target.destination)
		end
	end
end

function handleInteractableContact(A, B, BeginingContact)
	local other = nil;
	local target = nil
	local object = nil;
	for k, v in ipairs(interactables) do
		if(v.node.Name == A.Name) then
			target = v;
			other = B;
			object = v;
			break;
		elseif v.node.Name == B.Name then
			target = v;
			other = A;
			object = v;
			break;
		end
	end

	if other ~= nil and target ~= nil then
		if other.Name == target.node.observer.Name then
			onInteractableAction(object, target.node.observer, BeginingContact);
		end
	end
end


function onInteractableAction(obj, trigger, isBeginContact)
	--print("is Begin Contact? ", isBeginContact, "object: ", obj.name, "Trigger: ", trigger.Name)

	obj.isActivated = isBeginContact;
	--[[
	if(isBeginContact and obj.onBeginContact ~= nil) then
		obj.isActivated;
	elseif obj.onEndContact ~= nil then
		obj.onEndContact();
	end
	]]

end

function listenForKeyPress(key)
	for k, v in ipairs(interactables) do
		if v.isActivated and key == v.Activation_key and v.onBeginContact ~= nil then
			v.onBeginContact();
		end
	end
end


function CreateNPCS()
	local function saySomething(npc)
	end
	npcs = {
		{	name = "MR. Guy", pos = {x= 1, y = 6}, size = {x = 0.5, y = 1}, alias = "dummy", 
			icon = {alias = "inputs", frame = {x=323, y= 170, w= 16,h= 16}},
			onInteract = saySomething,
			text  = "Oh it's you!"
		}
	}
	for k, v in ipairs(npcs) do
		local e = Scene.CreateNPCNode(v.name, v.alias, v.icon.alias, "no script");

		e.transform.position:set(v.pos.x, v.pos.y);
		e.transform.size:set(v.size.x, v.size.y);
		e.transform:Center();
		e.textureScale:set(4, 2)
		e.rigidbody.bdyDef.type = 2;
		e.rigidbody.bdyDef.fixedRotation = true;
		e.rigidbody.fixDef.density = 1.0;
		e.rigidbody.fixDef.friction = 1.0;
		e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)
		e.icon.frame:set(v.icon.frame.x,v.icon.frame.y,v.icon.frame.w, v.icon.frame.h)
		e.prompter = mPlayer;
		e.text:setText(v.text, true)
		v.node = e;
	end

	
	DialogueText ={
		"Check out that box over there",
		"Well don't be shy, it don't bite now!",
		"Just press the interact key when you get there, same as you did with me.",
		"gosh your a timid fella aint ya..."
	}
	
	DialogueText2 ={
		"You did it!",
		"Now you can get to the next zone!",
		"If you try to interact with the box above us and see where that leads..."
	}
	DialogueText_index = 1;
	DialogueText_index2 = 1;
end

function onDialogueStart(NPC, Audience)

	if(big_obstacle_moved) then
		NPC.text:setText(DialogueText2[DialogueText_index2], true)
		
		DialogueText_index2 = DialogueText_index2 + 1;
		if DialogueText_index2 > #DialogueText2 then 
			DialogueText_index2 = 1
		end
	end
end

function onDialogueEnd(NPC, Audience )

	if(not big_obstacle_moved) then
		NPC.text:setText(DialogueText[DialogueText_index], true)

		DialogueText_index = DialogueText_index + 1;
		if DialogueText_index > #DialogueText then 
			DialogueText_index = 1
		end
	end
end