
function onSceneStart()
	Scene.setDrawToTarget(false)

	list = [[
		Todo's:	
		- Revamp the player controller. Less floaty, 
		  not as powerfull, better sprint feel.
		- simple inventory <-> tight coupling between 
          an 'item' and the player
		- main level sketches
		- loading screen's...
		- main/pause menu
		- - - save/load game state
		- - - update/modify system settings 
		(background color, keybindings, theme, audio, etc)
		- hub area
		- mission areas
		- - - Train/railway area
		- - - shopping district
		- - - residential district
		- dialogue system
]];

	list2 = [[	
	- also don't forget the gameplay!
]]

end

function onSceneEnd() 

end
function Update(dt) 

end

function Draw() 
end

function UIDraw() 
	

	Raylib.DrawTextEx("arial", list, 25, 25, 18,1.0, 0xffffffff)
	x, y = Raylib.MeasureText(list, 18, "arial");
	Raylib.DrawTextEx("comic", list2, 50 + x , 25, 18, 1, 0xffffffff )
end


function onKeyPress(key)
end

