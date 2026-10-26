local live = ...
local player = LifeHistoryGraph.GetPlayer()
if not player then return Def.ActorFrame {} end

local width = THEME:GetMetric("LifeHistory", "Width")
local height = THEME:GetMetric("LifeHistory", "Height")
local marginLeft = THEME:GetMetric("LifeHistory", "MarginLeft")
local marginBottom = THEME:GetMetric("LifeHistory", "MarginBottom")
local active = false
local lastStage, lastSecond, lastLife

local function Refresh(self, force)
	if not active then return end
	if LifeHistoryGraph.GetPlayer() ~= player then
		self:visible(false)
		return
	end
	self:visible(true)
	local stage
	if live then
		stage = STATSMAN:GetCurStageStats()
	else
		local screen = SCREENMAN:GetTopScreen()
		if screen then stage = screen:GetStageStats() end
	end
	if not stage then return end
	local stats = stage:GetPlayerStageStats(player)
	local seconds = LifeHistoryGraph.GetEndSecond(stage, stats)
	local life = stats:GetCurrentLife()
	if not force and stage == lastStage and seconds == lastSecond and life == lastLife then
		return
	end
	lastStage, lastSecond, lastLife = stage, seconds, life
	local values = stats:GetLifeRecord(seconds, LifeHistoryGraph.SampleCount)
	local body, line = LifeHistoryGraph.MakeVertices(values, width, height)
	self:GetChild("Body"):SetVertices(body)
	self:GetChild("Line"):SetVertices(line)
	self:GetChild("Value"):settext(string.format("%.0f%%", LifeHistoryGraph.ClampLife(life) * 100))
	self:GetChild("EndTime"):settext(string.format("%d:%02d", math.floor(seconds / 60), math.floor(seconds) % 60))
end

local t = Def.ActorFrame {
	Name = "LifeHistory",
	InitCommand = function(self)
		self:xy(SCREEN_LEFT + marginLeft, SCREEN_BOTTOM - marginBottom - height)
		-- Within the underlay: above the gameplay filter, below evaluation stats.
		self:draworder(live and 1 or -1)
	end,
	OnCommand = function(self)
		active = true
		self:stoptweening():diffusealpha(1)
		Refresh(self, true)
		if live then self:SetUpdateFunction(function(actor) Refresh(actor, false) end) end
	end,
	OffCommand = function(self)
		active = false
		self:SetUpdateFunction(nil)
		self:stoptweening():linear(0.2):diffusealpha(0)
	end,
	DoneLoadingNextSongMessageCommand = function(self)
		if live then Refresh(self, true) end
	end,
	LoadActor(THEME:GetPathG("LifeHistory", "backing")) .. {
		Name = "Backing",
		InitCommand = function(self)
			self:halign(0):valign(0):zoomto(width, height):diffusealpha(0.8)
		end,
	},
	Def.ActorMultiVertex {
		Name = "Body",
		Texture = THEME:GetPathG("LifeHistory", "body"),
		InitCommand = function(self)
			self:SetDrawState{Mode = "DrawMode_QuadStrip"}
			self:diffusealpha(0.9)
		end,
	},
	Def.ActorMultiVertex {
		Name = "Line",
		InitCommand = function(self)
			self:SetDrawState{Mode = "DrawMode_LineStrip"}
			self:SetLineWidth(2)
		end,
	},
	LoadFont("Common Condensed") .. {
		Name = "Title",
		InitCommand = function(self)
			self:xy(0, -12):halign(0):zoom(0.6):shadowlength(1)
			self:settext(THEME:GetString("LifeHistory", "Title"))
		end,
	},
	LoadFont("Common Condensed") .. {
		Name = "Value",
		InitCommand = function(self)
			self:xy(width, -12):halign(1):zoom(0.6):shadowlength(1)
		end,
	},
	LoadFont("Common Condensed") .. {
		Name = "StartTime",
		Text = "0:00",
		InitCommand = function(self)
			self:xy(0, height + 10):halign(0):zoom(0.5):shadowlength(1)
		end,
	},
	LoadFont("Common Condensed") .. {
		Name = "EndTime",
		InitCommand = function(self)
			self:xy(width, height + 10):halign(1):zoom(0.5):shadowlength(1)
		end,
	},
}

-- This event is sent after the record is written, so the endpoint is current.
t["LifeMeterChanged" .. ToEnumShortString(player) .. "MessageCommand"] = function(self)
	if live then Refresh(self, true) end
end

return t
