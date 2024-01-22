

function LoadFonts()
	mFonts = {
		{path = "C:/Windows/Fonts/arial.ttf", alias = "arial", id = -1},
		{path = "C:/Windows/Fonts/Rock.ttf", alias = "rockwell", id = -1},
		{path = "C:/Windows/Fonts/vivaldii.ttf", alias = "vivaldii", id = -1},
		{path = "C:/Windows/Fonts/HARLOWSI.ttf", alias = "harlow", id = -1},
		{path = "C:/Windows/Fonts/comic.ttf", alias = "comic", id = -1},
	}

	for i, v in ipairs(mFonts) do
		v.id = FontManager.AddFont(v.path, v.alias);
	end
end

function unLoadFonts()
	for i, v in ipairs(mFonts) do
		FontManager.RemoveFont(v.id);
	end
end
