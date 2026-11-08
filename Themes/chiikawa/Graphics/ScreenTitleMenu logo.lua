return Def.ActorFrame {
	LoadFont("Common Header") .. {
		Text="StepMania 6",
		InitCommand=function(self) self:diffuse(color("#4a3c40")):zoom(.95):maxwidth(SCREEN_WIDTH*.44/.95):shadowlength(0) end,
	},
	LoadFont("Common Normal") .. {
		Text="Find your rhythm.",
		InitCommand=function(self) self:y(64):zoom(.55):diffuse(color("#796970")):shadowlength(0) end,
	},
}
