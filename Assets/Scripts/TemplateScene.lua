
function onSceneStart()
	print("Revamped lua scripted scene!");
	onSceneStart2();

end


function onSceneStart2()
	print("Hello from LUA!! we will be making some nodes today!")

	gameObjects = {};
	math.randomseed(os.time());
	for i=1, 350, 1 do
		x = math.random(0, Raylib.GetScreenWidth())
		y = math.random(0, Raylib.GetScreenHeight())
		size = math.random(15, 100)
		size = size + math.random();

		local name = "cube" .. i;
		local e = test.Node(name, x, y, size, size);
		e.material:SetColorVec(math.random(), math.random(), math.random(), 0.66);
		e.transform.angle = math.random(-360, 360)
		x = math.random(-50, 50);
		y = math.random(-50, 50);

		av = math.random(-60, 60);


		e.AngularVelocity = av;
		e.material:SetTextureByAlias("CHECKER");
		--e.velocity:set(x, y);
		gameObjects[name] = e;
		gameObjects[name].velocity:set(x, y);

		
		x = math.random(-10, 10);
		y = math.random(-10, 10);
		e.material.uv_scroll:set(x, y);


	end

	gameObjects.Default = "one value";

	print("size? ", #gameObjects)

	for k, v in ipairs(gameObjects) 
	do
		print("Again we see", k, v)
	end
end

function onSceneEnd()
	print("Goodbye from LUA!!")

end

