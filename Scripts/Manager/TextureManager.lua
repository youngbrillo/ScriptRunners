
Textures = {}

function LoadTextures()

	local size = 8;
	Textures["CHECKER"] = 
		TextureManager.generateCheckered("CHECKER", 32, 32, size, size, 0x0000005f, 0xffffffff)
	print(">>>\tChecker texture id upon creation: "..Textures["CHECKER"])

end


function unLoadTextures()
	print(">>>\tChecker texture id right before deletion: "..Textures["CHECKER"])
	TextureManager.Remove(Textures["CHECKER"])
end