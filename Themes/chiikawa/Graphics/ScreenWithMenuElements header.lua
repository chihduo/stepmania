return Def.ActorFrame {
	Def.Quad {
		InitCommand=function(self) self:valign(0):zoomto(SCREEN_WIDTH,92):diffuse(color("#f7bbc8")):diffusebottomedge(color("#fbe2e6")) end,
	},
	Def.Quad {
		InitCommand=function(self) self:y(92):zoomto(SCREEN_WIDTH,3):diffuse(color("#cbe7d7")) end,
	},
	Def.Sprite {
		InitCommand=function(self) self:xy(-SCREEN_CENTER_X+56,48):diffuse(color("#796970")) end,
		OnCommand=function(self)
			local folder = "/Themes/default/Graphics/ScreenWithMenuElements header/"
			local path = folder .. SCREENMAN:GetTopScreen():GetName() .. " icon.png"
			if not FILEMAN:DoesFileExist(path) then path = folder .. "Generic icon.png" end
			self:Load(path)
		end,
	},
	LoadFont("Common Header") .. {
		Name="HeaderTitle", Text=Screen.String("HeaderText"),
		InitCommand=function(self) self:xy(-SCREEN_CENTER_X+110,49):halign(0):diffuse(color("#4a3c40")):shadowlength(0) end,
		UpdateScreenHeaderMessageCommand=function(self,param) self:settext(param.Header) end,
	},
}
