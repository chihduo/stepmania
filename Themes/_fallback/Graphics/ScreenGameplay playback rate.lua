local flawless = THEME:GetMetric("Judgment", "JudgmentW1Command")

local function HideRate(self)
	self:finishtweening():stopeffect():diffusealpha(0):visible(false)
end

return Def.ActorFrame {
	InitCommand = function(self)
		self:xy(SCREEN_LEFT + 16, SCREEN_BOTTOM - 16):zoom(1.5)
	end,
	LoadFont("ScreenGameplay", "playback rate") .. {
		Name = "Rate",
		InitCommand = function(self)
			self:horizalign(left):vertalign(bottom):shadowlength(0)
			self:textglowmode("TextGlowMode_Inner")
			HideRate(self)
		end,
		RateChangedCommand = function(self, param)
			-- Keep nonstandard rates precise while showing ordinary steps as 1.1x.
			local rate = string.format("%.3f", param.Rate):gsub("0+$", ""):gsub("%.$", ".0")
			self:finishtweening():stopeffect():visible(true):settext(rate .. "x")
			self:diffuse(JudgmentLineToColor("JudgmentLine_W1"))
			self:strokecolor(JudgmentLineToStrokeColor("JudgmentLine_W1"))
			-- Animate the child so judgment commands cannot move the corner anchor.
			flawless(self)
		end,
		ResetCommand = HideRate,
		OffCommand = HideRate,
	}
}
