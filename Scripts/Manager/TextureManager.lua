
Textures = {}

function LoadTextures()

	local size = 8;
	Textures["CHECKER"] = TextureManager.generateCheckered("CHECKER", 32, 32, size, size, 0x0000005f, 0xffffffff)
end


function unLoadTextures()
	TextureManager.Remove(Textures["CHECKER"])
end