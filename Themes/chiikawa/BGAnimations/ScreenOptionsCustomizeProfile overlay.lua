-- Keep the inherited profile editor's input and save behavior, and adapt its
-- text/cursors to the cream background. The fallback was designed for dark UI.
local ink = color("#4a3c40")
local editor = LoadActor("/Themes/_fallback/BGAnimations/ScreenOptionsCustomizeProfile overlay.lua")

return Def.ActorFrame {
	editor .. {
		OnCommand = function(self)
			for name, actor in pairs(self:GetChildren()) do
				if name == "menu_cursor" then
					actor:diffuse(color("#cbe7d7"))
				elseif name == "fader" then
					actor:diffuse(color("#fff8ee")):diffusealpha(0)
				elseif name:match("^menu_") or name:match("^value_") then
					actor:diffuse(ink)
				end
			end
			local keypad = self:GetChild("number_entry")
			for name, actor in pairs(keypad:GetChildren()) do
				if name == "cursor" then
					actor:diffuse(color("#f7bbc8"))
				elseif name == "value" then
					actor:diffuse(color("#2c7593"))
				elseif name == "prompt" or name:match("^num%d+$") then
					actor:diffuse(ink)
				end
			end
		end,
	},
	LoadFont("Common Header") .. {
		Text = GAMESTATE:GetEditLocalProfile():GetDisplayName(),
		InitCommand = function(self)
			self:xy(32,30):halign(0):zoom(0.6):maxwidth((SCREEN_WIDTH-64)/0.6)
			self:diffuse(ink):shadowlength(0)
		end,
	},
}
