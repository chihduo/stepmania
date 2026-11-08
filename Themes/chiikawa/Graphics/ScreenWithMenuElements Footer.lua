return Def.ActorFrame {
	Def.Quad {
		InitCommand=function(self) self:valign(1):zoomto(SCREEN_WIDTH,50):diffuse(color("#4a3c40")) end,
	},
	Def.Quad {
		InitCommand=function(self) self:y(-50):zoomto(SCREEN_WIDTH,3):diffuse(color("#cbe7d7")) end,
	},
}
