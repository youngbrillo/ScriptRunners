--[[
	objective:
		Test the TaskManager Node so that it:
			- resolves a delivery automatically when the package reaches the destination
			- guides the player to the package 
			- guides the player to the destination
]]
-- // -----------------------------------------------------------------------------
function onSceneStart() end
function onSceneEnd() end
function Update(dt) end
function Draw() end
function UIDraw() DrawSceneTitle() end
function onKeyPress(key) end
function onBeginContact(A, B) end
function onEndContact(A, B) end

-- // -----------------------------------------------------------------------------
function SpawnPlayer(x, y )
	mPlayer = Scene.CreatePlayerController("Player Controller", "dummy");
	--mPlayer = Scene.CreatePlayerController2("Player Controller", "dummy", "Player.ini");
	mPlayer.textureScale:set(4, 2)
	mPlayer:setPosition(x,y)

	for k, v in ipairs(npcs) do v.node.prompter = mPlayer; end
	for k, v in ipairs(interactables) do 	v.node:setObserver(mPlayer); end
	for k, v in ipairs(camControllers) do 	v.node:SetTarget(mPlayer);	end
end

function DrawSceneTitle()
	Raylib.DrawTextEx("arial", "Test: Objective / Task Mechanics", 25, Raylib.GetScreenHeight()-60, 32 ,1.0, 0xffffffff)
end