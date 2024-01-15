
Player = require("Scripts/Components/Player")

LIGHTGRAY = 0xC8C8C8FF
GRAY = 0x828282FF
SHADOW = 0x000000FF


function onSceneStart()
	print("Hello! Please Get the Player!");
	GenEnvironment2();

	--GenEnvironment();
	--GenPlayer();
end

function onSceneEnd() 
	RemoveBackGroundTextures();
end
function Update(dt) 
	--ScriptedUpdate(dt);

end

function Draw() 
	--ScriptedDraw()
end

function UIDraw() 
	--ScriptedDrawUI();
	DrawInstructions()
end


function onKeyPress(key)
	--ScriptedKeyPress(key);
end

function DrawInstructions()
	text = "Build a path to the upmost platform to proceed";
	local fontSize = 20;
	local pos = {x = 25, y = Raylib.GetScreenHeight() - fontSize};
	local _x, _y = Raylib.MeasureText(text, fontSize)
	Raylib.DrawRectangle(pos.x - fontSize/2, pos.y - (_y + fontSize/2), _x + fontSize, _y + fontSize, 0xCECE007F)
	Raylib.DrawText(text, pos.x, pos.y - _y, fontSize, 0xffffffff)


	instructionText = "Todo:\n- Add onBeginContact / onEndContact handlers in Lua\n- Simulate Collectables\n- Add Interactable objects (switches, levers, doors)\n - Scene Transition to another Scene";
	_x, _y = Raylib.MeasureText(instructionText, fontSize)
	pos = {x = Raylib.GetScreenWidth() - (_x + fontSize), y = Raylib.GetScreenHeight() - (_y + fontSize)};

	
	Raylib.DrawRectangle(pos.x - fontSize/2, pos.y - (fontSize/2), _x + fontSize, _y + fontSize, 0x00CE007F)
	Raylib.DrawText(instructionText, pos.x, pos.y , fontSize, 0xffffffff)
end

function LoadBackgroundTextures()
	bgTextures = {};
	local base_path = "Assets/Textures/";
	local cloud_path = "clouds_craft-pixel/Clouds/Clouds 4/"
		  cloud_path = "clouds_craft-pixel/city-backgrounds/city 1/"
		  --cloud_path = "clouds_craft-pixel/city-backgrounds/city 6/"
	table.insert(bgTextures, {id = TextureManager.Add(base_path..cloud_path.."1", "bg_1"), name = "bg_1"});
	table.insert(bgTextures, {id = TextureManager.Add(base_path..cloud_path.."2", "bg_2"), name = "bg_2"});
	table.insert(bgTextures, {id = TextureManager.Add(base_path..cloud_path.."3", "bg_3"), name = "bg_3"});
	table.insert(bgTextures, {id = TextureManager.Add(base_path..cloud_path.."4", "bg_4"), name = "bg_4"});
	
	table.insert(bgTextures, {id = TextureManager.Add(base_path..cloud_path.."5", "bg_5"), name = "bg_5"});
	table.insert(bgTextures, {id = TextureManager.Add(base_path..cloud_path.."6", "bg_6"), name = "bg_6"});
	

	print(">>>>>>>>>>>>>>>");
	for k, v in ipairs(bgTextures)
	do
		print(">>>>>", k, v.id, v.name);
		local p = Scene.CreateNode2d("Background-"..v.name);
		p.transform.size.x = 100;
		p.transform.size.y = 40;
		p.transform:Center();
		p.material:SetTextureByAlias(v.name)
		p.material.uv_scroll:set(2 * (k-1), 0)

	end
	print(">>>>>>>>>>>>>>>\n\n\n\n");



	
	backgrounds_initiated = true;
end

function RemoveBackGroundTextures()
	if backgrounds_initiated then
		for k, v in ipairs(bgTextures)
		do
			TextureManager.Remove(v.id);
		end
	end
end

function setCheckerBackGround()
	local p = Scene.CreateNode2d("Background");
	p.transform.size.x = 100;
	p.transform.size.y = 40;
	p.transform:Center();
	p.material:SetColor(0X00ffff5e)
	p.material:SetTextureByAlias("CHECKER")
	p.material.source.width = p.material.source.width  * p.transform.size.x / p.transform.size.y * 4;
	p.material.source.height =p.material.source.height * 4;
	p.material.uv_scroll:set(10, 5)
end
function GenEnvironment2()
	LoadBackgroundTextures();
	elements =  {
         {name="floor", x = 00,	y = 20,	w=100,  h=.5 , blocking =true, color = 0x6ca3c3ff , hasShadow = false, dynamic = false}
        ,{name="center platform", x = 0,	y = 5, w=040,  h=.5 , blocking =true, color = 0x6ca3c3ff , hasShadow = true, dynamic = false}
        ,{name="center platform 1", x = -22.5,	y = 10, w=5,  h=.5 , blocking =true, color = 0x6ca3c3ff , hasShadow = true, dynamic = false}
        ,{name="center platform 2", x = -17,	y = 13, w=5,  h=.5 , blocking =true, color = 0x6ca3c3ff , hasShadow = true, dynamic = false}
        ,{name="center platform 3", x = -12,	y = 15, w=5,  h=.5 , blocking =true, color = 0x6ca3c3ff , hasShadow = true, dynamic = false}
        ,{name="center platform 4", x = -7,	y = 17, w=5,  h=.5 , blocking =true, color = 0x6ca3c3ff , hasShadow = true, dynamic = false}
        ,{name="left platform", x = -30,	y = 12.25, w=010,  h=15 , blocking =true, color = 0x6ca3c3ff , hasShadow = true,dynamic = false}
        ,{name="right platform", x = 30,	y = 12.25, w=010,  h=15 , blocking =true, color = 0x6ca3c3ff , hasShadow = true,dynamic = false }
        ,{name="top platform", x = 0,	y = -7, w=010,  h=.5 , blocking =true, color = 0xff0000ff , hasShadow = true, dynamic = false }
        ,{name="left bounds", x = -50,	y = 9.75, w=0.5,  h=20 , blocking =true, color = 0x000000ff , hasShadow = true, dynamic = false }
        ,{name="left bounds", x = 50,	y = 9.75, w=0.5,  h=20 , blocking =true, color = 0x000000ff , hasShadow = true, dynamic = false }
	}
	for k, v in ipairs(elements) do
		local e = Scene.CreateNode2d(v.name);
		e.transform.position:set(v.x, v.y);
		e.transform.size:set(v.w, v.h);
		e.transform:Center();
		e.material:SetColor(v.color);
		e.solid = v.blocking
		if(v.dynamic) then
			e.rigidbody.bdyDef.type = 2
			e.rigidbody.fixDef.density = 1.0;
		end
		e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)
	end



	TextureManager.Add("Assets/Textures/dummy", "dummy")
	mPlayer = Scene.CreatePlayerController("Player Controller", "dummy");
	mPlayer.textureScale:set(4, 2)

	App.GetCamera().zoom = 34;
	--GenerateBreakableBoxes();
	AddCameraControllers();
	BuildPullySystem();
end				  

function AddCameraControllers()

	local camControllers = 
	{
		--{pos = {x = 0, y = 2.5}, size = {x=5, y=5}, onEnter = 84, onExit = 34},
		{pos = {x = 0, y = 5-25/2}, size = {x=40, y=25}, onEnter = 34, onExit = 65},
		{pos = {x = 30, y = 2.5}, size = {x=10, y=5}, onEnter = 19, onExit = 34},
		{pos = {x = 0, y = 15}, size = {x=50, y=10}, onEnter = 34, onExit = 34},
		{pos = {x = -23.5, y = 3}, size = {x=21, y=13}, onEnter = 34, onExit = 19},
		{pos = {x = -42.5, y = 16}, size = {x=15, y=8}, onEnter = 85, onExit = 34}
	}

	for k, v in ipairs(camControllers) do
	
		local e  = Scene.CreateCameraController2d("cam controller ".. k);
		e.transform.position:set(v.pos.x, v.pos.y);
		e.transform.size:set(v.size.x, v.size.y);
		e.transform:Center();
		e.material:SetColorVec(1,1,1,0.5)
		e:SetTarget(mPlayer);
		e:Standardize();
		e.onEnter.zoom = v.onEnter;
		e.onExit.zoom = v.onExit;
		e.visible = false;
	end
end


function  GenerateBreakableBoxes()
	xLimit = {min = -45, max = 45}
	yLimit = {min = -15, max = 15}
	sLimit = {min = 0, max = 1}

	for i = 1, 100, 1 do
		local e = Scene.CreateNode2d("box-"..i);
		e.transform.position:set(
			math.random(xLimit.min, xLimit.max),
			math.random(yLimit.min, yLimit.max)
		)
		e.transform.size:set(
			math.random(sLimit.min, sLimit.max),
			math.random(sLimit.min, sLimit.max)
		)
		e.transform.size.x = e.transform.size.x + math.random();
		e.transform.size.y = e.transform.size.y + math.random();
		if e.transform.size.x < 0.1 then e.transform.size.x = 0.1;end
		if e.transform.size.y < 0.1 then e.transform.size.y = 0.1;end
		e.transform:Center();
		e.rigidbody.bdyDef.type = 2;
		e.rigidbody.fixDef.density = 1.0;
		e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)
		e.material:SetColorVec(math.random(), math.random(), math.random(), 0.7)
	end
end
function BuildPullySystem()
	
	local a = Scene.CreateNode2d("west lift");
			a.transform.position:set(-7.5, 0);
			a.transform.size:set(5, 0.5)
			a.transform:Center();
			a.rigidbody.bdyDef.type = 2;
			a.rigidbody.bdyDef.fixedRotation = true;
			a.rigidbody.fixDef.density = 10.0;
			a.rigidbody:SetBody(Scene.GetWorld(), a.transform, 0)
			a.material:SetColorVec(234/255, 16/255, 98/255, 0.7)

			
	
	local b = Scene.CreateNode2d("east lift");
			b.transform.position:set(7.5, 0);
			b.transform.size:set(5, 0.5)
			b.transform:Center();
			b.rigidbody.bdyDef.type = 2;
			b.rigidbody.bdyDef.fixedRotation = true;
			b.rigidbody.fixDef.density = 10.0;
			b.rigidbody:SetBody(Scene.GetWorld(), b.transform, 0)
			b.material:SetColorVec(234/255, 16/255, 98/255, 0.7)

	local rocks = {
		{name = "rock 1", x = -7.5, y = -6, w = 3, h = 3, density = 2.5},
		{name = "rock 2", x = 7.5, y = -26, w = 3, h = 3, density = 5}
	};

	for k, v in ipairs(rocks) do
		local e = Scene.CreateNode2d(v.name);
			e.transform.position:set(v.x, v.y);
			e.transform.size:set(v.w, v.h)
			e.transform:Center();

			e.rigidbody.bdyDef.type = 2;
			e.rigidbody.fixDef.density = 2.5;
			e.rigidbody:SetBody(Scene.GetWorld(), e.transform, 0)
			e.material:SetColorVec(0, 16/255, 98/255, 0.7)
	end


	b2d.CreatePullyJoint( Scene.GetWorld(), a, b, 
		{
			x1 = -7.5, y1 = -0.25, x2 = 7.5, y2 = -0.25, 
			x3 = -7.5, y3 = -7.5, x4 = 7.5, y4 = -7.5, 
			ratio = 1.0
		} 
	);
end


-- Scripted functions -- /////////////////////////////////////////////////////////////////////////////////////////////

function GenEnvironment()
	envSpan = 100;
	envHeight = 40;
	Ground = {
		x = 0, 
		y = 40, 
		w = envSpan, 
		h = 40,
		color = 0x00ff00ff
	}
	local width = Raylib.GetScreenWidth()
	local height = Raylib.GetScreenHeight()
	envElements = {
       -- { x = 00,	y = 00,	w=envSpan,  h=envHeight , blocking =false,color = LIGHTGRAY },
        { x = 00,	y = envHeight,	w=envSpan,  h=5 , blocking =true, color = GRAY , hasShadow = false},
        { x = 30,	y = 20, w=040,  h=01 , blocking =true, color = GRAY , hasShadow = true},
        { x = 25,	y = 30, w=010,  h=01 , blocking =true, color = GRAY , hasShadow = true},
        { x = 65,	y = 30, w=010,  h=01 , blocking =true, color = GRAY , hasShadow = true}
	}

	shadow_offset = {x = .5, y = 0.5}

	App.GetCamera().zoom = 8;
	App.GetCamera().target:set(envSpan * 0.5, envHeight - 10);

	local p = Scene.CreateNode2d("Background");
	p.transform.size.x = envSpan;
	p.transform.size.y = envHeight;
	p.transform:Align(0, 0);
	p.material:SetColor(0X00ffffff)
	p.material:SetTextureByAlias("CHECKER")
	p.material.source.width = p.material.source.width  * p.transform.size.x / p.transform.size.y;
	p.material.uv_scroll:set(10, 5)
	p.visible = true;
end

function ScriptedUpdate( dt )
	for i, rect in ipairs(envElements) do
		if	(rect.blocking and 
			rect.x <= Player.p.transform.position.x and
            rect.x + rect.w >= Player.p.transform.position.x and
            rect.y >= Player.p.transform.position.y and
            rect.y <=  Player.p.transform.position.y + Player.speed.y * dt
		)
		then
			Player.setGrounded(true)
			--Player.p.material:SetColor(0xffff00ff)
		end
	end
	Player.Update(dt);
end

function ScriptedDraw() 

	for i, rect in ipairs(envElements) do
		if rect.hasShadow then Raylib.DrawRectangle(rect.x + shadow_offset.x, rect.y + shadow_offset.y, rect.w, rect.h, SHADOW) end
		Raylib.DrawRectangle(rect.x, rect.y, rect.w, rect.h, rect.color)
	end

	Raylib.DrawLine(Ground.x, Ground.y, Ground.w, Ground.h, Ground.color);
end
function ScriptedDrawUI()
	Player.Draw();
end

function ScriptedKeyPress( key )
	Player.HandleKeyPress(key);
end

function GenPlayer()
	Player.Create();
	Player.p.transform.position:set(50, 40);
	Player.p.transform.size:set(2, 4);
	Player.p.transform:Align(0.5, 1);
end


