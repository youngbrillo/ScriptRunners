--[[
	objective:
		Test the TaskManager Node so that it:
			- resolves a delivery automatically when the package reaches the destination
			- guides the player to the package 
			- guides the player to the destination
]]
-- // -----------------------------------------------------------------------------
function onSceneStart() buildLevel(); end
function onSceneEnd() end
function Update(dt) end
function Draw() end
function UIDraw() DrawSceneTitle() end
function onKeyPress(key) HandleKeyPress(key); end
function onBeginContact(A, B) handleBeginContact(A, B);end
function onDialogueStart(speaker, audience) handleDialogueStart(speaker, audience); end
function onDialogueEnd(speaker, audience) handleDialogueEnd(speaker, audience); end


function onEndContact(A, B) end

-- // -----------------------------------------------------------------------------


function HandleKeyPress(key) triggers.onKeyPress(key);end
function handleBeginContact(A, B) triggers.onBeginContact(A, B); end
function SpawnPlayer(x, y )
	mPlayer = Scene.CreatePlayerController("Player Controller", "dummy");
	mPlayer.textureScale:set(4, 2)
	mPlayer:setPosition(x,y)
	for k, v in ipairs(NPCs) do v.node.prompter = mPlayer; end
	for k, v in ipairs(triggers) do v.node:setObserver(mPlayer); end
	--for k, v in ipairs(camControllers) do 	v.node:SetTarget(mPlayer);	end
end

function DrawSceneTitle()
	Raylib.DrawTextEx("arial", "Test: Objective / Task Mechanics", 25, Raylib.GetScreenHeight()-60, 32 ,1.0, 0xffffffff)
end

function buildLevel()
	math.randomseed(os.time());
	TextureManager.Add("Assets/Textures/dummy", "dummy");
	TextureManager.Add("Assets/Textures/kenny/inputs", "inputs")
	buildGround();
	buildTriggers();
	buildNPCs();
	SpawnPlayer(0, -10)
	App.GetCamera().zoom = 25.0;
end

function buildGround()
	ground = Scene.CreateNode2d("ground");
	ground.visible = false;
	ground.rigidbody:Init(Scene.GetWorld());
	groundElements = {
		{name="ground", p1 = {x = -50, y = 0}, p2 = {x = 50, y = 0}},
		{name="platform", p1 = {x = 1, y = -3}, p2 = {x = 3, y = -3}, p0 = {x=0, y = 0}, p3 = {x=1, y = 0}},
		{name="ramp", p1 = {x = -10, y = -2}, p2 = {x = -5, y = -0}, p0 = {x=0, y = 0}, p3 = {x=1, y = 0}},
		{name="ramp-platform", p2 = {x = -10, y = -2}, p1 = {x = -15, y = -2}, p0 = {x=0, y = 0}, p3 = {x=1, y = 0}},
		{name="west wall", p1 = {x = -50 , y = 0}, p2 = {x = -50, y = -20}},
		{name="east wall", p1 = {x = 50 , y = 0}, p2 = {x = 50, y = -20}},
		{name="step-01",  p1 = {x = 5, y = -3}, p2 = {x = 8, y = -3}		},
		{name="step-01",  p1 = {x = 8, y = -3}, p2 = {x = 12, y = -5}		},
		{name="step-01",  p1 = {x = 12, y = -5}, p2 = {x = 20, y = -5}		},
		{name="step-01",  p1 = {x = 20, y = -5}, p2 = {x = 20, y = -7}		},
		{name="step-01",  p1 = {x = 20, y = -7}, p2 = {x = 50, y = -7}		},
		{name="step-01",  p1 = {x = 12, y = -10}, p2 = {x = 16, y = -10}		},
		{name="step-01",  p1 = {x = 4, y = -12}, p2 = {x = 8, y = -12}		},
		{name="step-01",  p1 = {x = 22, y = -11}, p2 = {x = 30, y = -11}		},
		{name="step-01",  p1 = {x = 30, y = -11}, p2 = {x = 30, y = -12}		},
		{name="step-n",  p1 = {x = 40, y = -16}, p2 = {x = 50, y = -16}		},
		{name="step-n",  p1 = {x = 40, y = -16}, p2 = {x = 40, y = -14}		},
		{name="step-n",  p1 = {x = 30, y = -12}, p2 = {x = 35, y = -12}		},
		{name="step-n",  p1 = {x = 35, y = -12}, p2 = {x = 40, y = -14}		},
	}
	for i, v in ipairs(groundElements) do
		if v.p0 ~= nil and v.p3 ~= nil then 
			v.fixture = ground.rigidbody.body:AddOneSidedEdge(v.p0.x, v.p0.y, v.p1.x, v.p1.y, v.p2.x, v.p2.y, v.p3.x, v.p3.y);
		else
			v.fixture = ground.rigidbody.body:AddTwoSidedEdge(v.p1.x, v.p1.y, v.p2.x, v.p2.y);
		end
	end
end

function CreateObject(name, settings)
	local e = Scene.CreateNode2d(name or "Object");
		e.transform.position:set(settings.x, settings.y);
		e.transform.size:set(settings.w, settings.h);
		e.transform:Center();
		e.material:SetColor(settings.color or 0x000000ff);

		e.rigidbody.bdyDef.type = 2;
		e.rigidbody.fixDef.density = 1;
		e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0);

		return e;
end

function CreateObject_Interactable(name, v, icon, observer )
	local e = Scene.CreateInteractableNode(name or "Interactable");
		e.transform.position:set(v.x, v.y);
		e.transform.size:set(v.w, v.h)
		e.transform:Center();
		e.material:SetColor(v.color or 0xffffff7a);
		e.rigidbody.bdyDef.type = 1;
		e.rigidbody.fixDef.isSensor = true;
		e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)
		e:setIconDestination(0, -(e.transform.size.y + 0.5), 1.0, 1.0)
		e:setIconTexture(icon.texture)
		e:setIconFrame(icon.frame.x, icon.frame.y, icon.frame.w, icon.frame.h)

		if(observer ~= nil) then
			e:setObserver(observer);
		end

		return e;
end


function buildTriggers()
	local icon ={texture="inputs", frame = {x=323, y= 170, w= 16,h= 16}}
	
	local function beginMission(trigger)
		if(trigger.package ~= nil) then trigger.package.alive = false; end
		if(trigger.destination ~= nil) then trigger.destination.alive = false; end
		trigger.began = true; trigger.finished = false;
		trigger.package = CreateObject("package", trigger.spawn)
		local settings = 
		{
			x = math.random(trigger.bounds.minX, trigger.bounds.maxX),
			y = math.random(trigger.bounds.minY, trigger.bounds.maxY),
			w = 2,
			h = 2
		}
		trigger.destination = CreateObject_Interactable("Destination", settings, {texture="inputs", frame = {x=323, y= 170, w= 16,h= 16}}, trigger.package)
	end
	local function missionBeginContact(this, A, B)
		if(
			this.began and not 
			this.finished and
			this.destination.isInteractive
		) then
			print("the package and the destination have both met")
			this.finished = true;
			this.package.alive = false;
			this.destination.alive = false;
			this.package = nil;
			this.destination = nil;
		end
	end
	triggers = {
		{name = "mission Start", x = 1.5, y = -4, w = 1, h = 2,  began = false, finished = false, onTrigger = beginMission,  onBeginContact = missionBeginContact, spawn = {x = -9.5, y = -3, w = 1, h = 1}, bounds = {minX = 0, minY = -1, maxX = 15, maxY = -1}},
	}
	for i, v in ipairs(triggers) do
		v.node = CreateObject_Interactable(v.name, v, icon, nil);
	end
	--USE KEY
	triggers.Activation_key = 69; -- KEY_E
	--onKeyPress
	triggers.onKeyPress = function(key)
		for i , v in ipairs(triggers) do
			if(v.node.isInteractive and key == triggers.Activation_key) then
				v.onTrigger(v);
			end
		end
	end
	--onBeginContact
	triggers.onBeginContact = function(A, B)
		for i , v in ipairs(triggers) do
			if(v.onBeginContact ~= nil) then v.onBeginContact(v, A, B) end;
		end
	end
end

function buildNPCs()
	NPCs = {
		{
			name = "Giver",		text="Talk to me for work.", font = "arial",	x = -14, y = -5.0, direction = 1},
		{name = "Red Fan",		text="I Like Red boxes.", font = "arial",		x = 26, y = -12.0, color = 0xff3456ff},
		{name = "Green Fan",	text="I Like Green boxes.", font = "arial",		x = 6, y = -13.0, color = 0x12ff56ff},
		{name = "Blue Fan",		text="I Like Blue boxes.", font = "arial",		x = 35, y = -7.0, color = 0x1234ffff},
		{name = "White Fan",	text="I Like White boxes.", font = "arial",		x = 45, y = -18.0, color = 0xffffffff},
		{name = "Black Fan",	text="I Like Black boxes.", font = "arial",		x = 30, y = -5.0, color = 0x000000ff},
	}
	local icon = {alias = "inputs", frame = {x=323, y= 170, w= 16,h= 16}};
	for k, v in ipairs(NPCs) do
		local e = Scene.CreateNPCNode(v.name, "dummy", icon.alias, "npc.script.lua");
			e.transform.position:set(v.x, v.y);
			e.transform.size:set(0.5, 1);
			e.transform:Center();
			e.textureScale:set(4, 2)
			e.material:SetColor(v.color or 0xffffffff);
			e.rigidbody.bdyDef.type = 2;
			e.rigidbody.bdyDef.fixedRotation = true;
			e.rigidbody.fixDef.density = 1.0;
			e.rigidbody.fixDef.friction = 1.0;
			e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)
			e.icon.frame:set(icon.frame.x,icon.frame.y,icon.frame.w, icon.frame.h)
			e.text:setText(v.text)
			e.text.fontSize = 32
			e.text:SetFont(v.font)
			e.material.direction.x = v.direction or -1;
			b2d.AddCircleSensor(e.rigidbody, 1.25)
			v.node = e;
	end
end

function handleDialogueStart(speaker, audience)

end

function handleDialogueEnd(speaker, audience)

end