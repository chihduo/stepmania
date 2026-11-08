local live = ...
local actor = LoadActor("/Themes/default/Graphics/LifeHistory graph.lua", live)
if not live then
	local originalOn = actor.OnCommand
	actor.OnCommand = function(self)
		if originalOn then originalOn(self) end
		-- Dark labels stay readable on cream; keep the original line/fill alpha.
		for _,name in ipairs({"Title", "Value", "StartTime", "EndTime"}) do
			local label = self:GetChild(name)
			if label then label:diffuse(color("#4a3c40")):diffusealpha(.8):shadowlength(0) end
		end
	end
end
return actor
