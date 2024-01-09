
Player = require("Scripts/Scenes/Player")

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

end
function Update(dt) 
	--ScriptedUpdate(dt);

end

function Draw() 
	--ScriptedDraw()
end

function UIDraw() 
	--ScriptedDrawUI();
end


function onKeyPress(key)
	--ScriptedKeyPress(key);
end


function GenEnvironment2()
	local p = Scene.CreateNode2d("Background");
	p.transform.size.x = 100;
	p.transform.size.y = 40;
	p.transform:Center();
	p.material:SetColor(0X00ffff5e)
	p.material:SetTextureByAlias("CHECKER")
	p.material.source.width = p.material.source.width  * p.transform.size.x / p.transform.size.y * 4;
	p.material.source.height =p.material.source.height * 4;
	p.material.uv_scroll:set(10, 5)
	p.visible = true;
	p.solid	 = false;

	
	elements =  {
        {name="floor", x = 00,	y = 20,	w=100,  h=.5 , blocking =true, color = GRAY , hasShadow = false, dynamic = false},
        {name="center platform", x = 0,	y = 5, w=040,  h=.5 , blocking =true, color = GRAY , hasShadow = true, dynamic = false},
        {name="left platform", x = -30,	y = 10, w=010,  h=.5 , blocking =true, color = 0x9f835cff , hasShadow = true,dynamic = false},
        {name="right platform", x = 30,	y = 10, w=010,  h=.5 , blocking =true, color = 0x6ca3c3ff , hasShadow = true,dynamic = false },
        {name="top platform", x = 0,	y = -7, w=010,  h=.5 , blocking =true, color = 0xff0000ff , hasShadow = true, dynamic = false }
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

	mPlayer = Scene.CreatePlayerController();


	App.GetCamera().zoom = 12;

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


