local Color1 = color(Var "Color1");
local file = Var "File1"

-- A theme may animate its missing-song-art fallback while retaining a bitmap
-- for thumbnails and static-background mode. Keep the normal movie clock and
-- focus handlers below so live rate changes, pause and replay stay synchronized.
if THEME:HasMetric("Background", "FallbackMovie")
	and file == THEME:GetPathG("Common", "fallback background")
	and PREFSMAN:GetPreference("SongBackgrounds")
	and not GAMESTATE:GetSongOptionsObject("ModsLevel_Current"):StaticBackground() then
	local movie = THEME:GetMetric("Background", "FallbackMovie")
	if type(movie) == "string" and movie ~= "" and FILEMAN:DoesFileExist(movie) then
		file = movie
	end
end

local t = Def.ActorFrame {
	LoadActor(file) .. {
		OnCommand=function(self)
			self:xy(SCREEN_CENTER_X,SCREEN_CENTER_Y)
			self:scale_or_crop_background();
			self:diffuse(Color1)
			self:effectclock("music")
			-- Explanation in StretchNoLoop.lua.
			if self.GetTexture then
				self:GetTexture():rate(self:GetParent():GetUpdateRate())
			end
		end;
		GainFocusCommand=cmd(play);
		LoseFocusCommand=cmd(pause);
	};
};

return t;
