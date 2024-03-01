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
function onKeyPress(key) 
	triggers.onKeyPress(key);
end
function onBeginContact(A, B)
	triggers.onBeginContact(A, B);
end
function onEndContact(A, B) end

-- // -----------------------------------------------------------------------------
function SpawnPlayer(x, y )
	mPlayer = Scene.CreatePlayerController("Player Controller", "dummy");
	mPlayer.textureScale:set(4, 2)
	mPlayer:setPosition(x,y)
	--for k, v in ipairs(npcs) do v.node.prompter = mPlayer; end
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
		{name="goalPost-1", p1 = {x = 10 , y = 0}, p2 = {x = 10, y = -2}, p0 = {x=0, y = 0}, p3 = {x=1, y = 1}},
		{name="goalPost-2", p1 = {x = 15 , y = 0}, p2 = {x = 15, y = -5}, p0 = {x=0, y = 0}, p3 = {x=1, y = 1}},
		{name="goalPost-3", p1 = {x = 20 , y = 0}, p2 = {x = 20, y = -8}},
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
	triggers = {
		{name = "mission Start", x = 1.5, y = -4, w = 1, h = 2, 
			began = false,
			finished = false,
			onTrigger = beginMission, 
			onBeginContact = missionBeginContact,
			spawn = {x = -9.5, y = -3, w = 1, h = 1}, bounds = {minX = 0, minY = -1, maxX = 15, maxY = -1}
		},
	}
	for i, v in ipairs(triggers) do
		v.node = CreateObject_Interactable(v.name, v, icon, nil);
	end
	triggers.Activation_key = 69; -- KEY_E

	triggers.onKeyPress = function(key)
		for i , v in ipairs(triggers) do
			if(v.node.isInteractive and key == triggers.Activation_key) then
				v.onTrigger(v);
			end
		end
	end

	triggers.onBeginContact = function(A, B)
		for i , v in ipairs(triggers) do
			if(v.onBeginContact ~= nil) then v.onBeginContact(v, A, B) end;
		end
	end
end

function beginMission(trigger)
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

function missionBeginContact(this, A, B)
	
	if(
		this.began and not 
		this.finished and
		(A:GetID() == this.package:GetID() or B:GetID() == this.package:GetID()) and
		(A:GetID() == this.destination:GetID() or B:GetID() == this.destination:GetID()) 
	) then
		print("the package and the destination have both met")
		this.finished = true;
		this.package.alive = false;
		this.destination.alive = false;
		this.package = nil;
		this.destination = nil;
	end
end