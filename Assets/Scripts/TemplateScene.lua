

function onSceneStart()
	print("Hello from LUA!! we will be making some nodes today!")
	Node = test.Node();
	math.randomseed(os.time());
	createCheckeredNodes(350);
end

function onSceneEnd()
	print("Goodbye from LUA!!")

end

function createCheckeredNodes( limit )
	for i=1, limit, 1 do
		local x = math.random(0, Raylib.GetScreenWidth())
		local y = math.random(0, Raylib.GetScreenHeight())
		local size = math.random(15, 100); size = size + math.random();
		local ang = math.random(-360, 360)
		local uvx = math.random(-10, 10);
		local uvy = math.random(-10, 10);
		local av = math.random(-60, 60);
		local name = "node " .. i;
			-- node creation 
		local e = Node:Create(name, x, y, size, size);

			-- material properties
		e.material:SetColorVec(math.random(), math.random(), math.random(), 0.66);
		e.material:SetTextureByAlias("CHECKER");
		e.material.uv_scroll:set(uvx, uvy);
			-- transform properties
		e.transform.angle = ang;

		
			-- node properties
		e.AngularVelocity = av;
		e.velocity:set(math.random(-10, 10),  math.random(-10, 10));
			-- attach to script table
	end
end

