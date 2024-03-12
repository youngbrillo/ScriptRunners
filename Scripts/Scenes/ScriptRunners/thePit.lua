-- goal of this level/scene is to escape the pit
-- climb out and progress on your path.
-- base functions -- ///////////////////////////////////////////////////////////////////////////
function onSceneStart() 
    Begin(); end
function onSceneEnd() 
    End(); end
function Update(deltaTime) end
function Draw() end
function UIDraw() end
function onKeyPress(key) end
function onBeginContact(A, B) end
function onEndContact(A, B) end

-- level functions -- ///////////////////////////////////////////////////////////////////////////
function Begin()
	math.randomseed(os.time());
	TextureManager.Add("Assets/Textures/dummy", "dummy");
    LoadBackGrounds();
    BuildLevel();
	SpawnPlayer(0, -20)
	App.GetCamera().zoom = 20.0;

end
function End()
    clearBackGrounds();
end 

function LoadBackGrounds() end
function clearBackGrounds() end

function GenerateGround()
	ground = Scene.CreateNode2d("ground");
	ground.visible = false;
	ground.rigidbody:Init(Scene.GetWorld());
    local exit_top = -15
	groundEdges = {
        {p1 = {x = -25, y = 10}, p2 = {x = 25, y = 10}}, -- ground
        {p1 = {x = -25, y = 10}, p2 = {x = -25, y = -50}}, -- left wall
        {p1 = {x = 25, y = -45}, p2 = {x = 25, y = -50}},    --right wall top
        {p1 = {x = 25, y = 10}, p2 = {x = 25, y = exit_top}},    --right wall
        {p1 = {x = 25, y = exit_top}, p2 = {x = 50, y = exit_top}},    -- path out
    }
    for i , v in ipairs(groundEdges) do
        if v.p0 ~= nil and v.p3 ~= nil then 
			v.fixture = ground.rigidbody.body:AddOneSidedEdge(v.p0.x, v.p0.y, v.p1.x, v.p1.y, v.p2.x, v.p2.y, v.p3.x, v.p3.y);
        else
			v.fixture = ground.rigidbody.body:AddTwoSidedEdge(v.p1.x, v.p1.y, v.p2.x, v.p2.y);
        end
    end
end

function BuildLevel()
    GenerateGround();
    local objects = {
        {name = "box 1", x = -10, y = 0, w = 3, h = 1, density = 10},
        {name = "box 2", x = -10, y = -3, w = 2, h = 2, density = 20},
    };

    for i=1,10 do
        table.insert(objects, {
            x=math.random(-25, 25), 
            y=math.random(-35, 5), 
            w= math.random(1, 10) + math.random(),
            h= math.random(1, 10) + math.random()
        })
    end


    for i, v in ipairs(objects) do
        v.name = v.name or "node - ".. i;
        local e = CreatePhysNode(v);
        v.node = e;
    end
end


function CreatePhysNode(definition)
    local e = Scene.CreateNode2d(definition.name);
    e.transform.position:set(definition.x, definition.y);
    e.transform.size:set(definition.w, definition.h)
    e.material:SetColor(definition.c or 0xa7a7a77a);
    e.transform:Center();
    e.rigidbody.bdyDef.type = definition.bodyType or 2
    e.rigidbody.fixDef.density = definition.density or 1;
    e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0);
    definition.node = e;
    return e;
end

function SpawnPlayer(x, y )
	mPlayer = Scene.CreatePlayerController("Player Controller", "dummy");
	mPlayer.textureScale:set(4, 2)
	mPlayer:setPosition(x,y)
	--for k, v in ipairs(NPCs) do v.node.prompter = mPlayer; end
	--for k, v in ipairs(triggers) do v.node:setObserver(mPlayer); end
	--for k, v in ipairs(camControllers) do 	v.node:SetTarget(mPlayer);	end
end