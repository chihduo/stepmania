-- Run from the repository root with Lua 5.1. No display or saved game data needed.
dofile('Themes/default/Scripts/05 LifeHistoryGraph.lua')
local G = LifeHistoryGraph
local humans, enabled, demo, course = {'PlayerNumber_P1'}, {'PlayerNumber_P1'}, false, nil
GAMESTATE = {
	GetHumanPlayers = function() return humans end,
	GetEnabledPlayers = function() return enabled end,
	IsDemonstration = function() return demo end,
	GetCurrentCourse = function() return course end,
}
assert(G.GetPlayer() == 'PlayerNumber_P1')
humans, enabled = {'PlayerNumber_P2'}, {'PlayerNumber_P2'}
assert(G.GetPlayer() == 'PlayerNumber_P2')
enabled = {'PlayerNumber_P1', 'PlayerNumber_P2'}
assert(G.GetPlayer() == nil)
humans = enabled
assert(G.GetPlayer() == nil)
humans, enabled = {'PlayerNumber_P1'}, {'PlayerNumber_P1'}
demo = true
assert(G.GetPlayer() == nil)
demo = false
course = {IsEndless = function() return true end}
assert(G.GetPlayer() == nil)
course = {IsEndless = function() return false end}
assert(G.GetPlayer() == 'PlayerNumber_P1')
course = nil

local body, line = G.MakeVertices({-0.1, 0.5, 1.1}, 200, 64)
assert(#body == 6 and #line == 3)
assert(line[1][1][1] == 0 and line[1][1][2] == 64)
assert(line[2][1][1] == 100 and line[2][1][2] == 32)
assert(line[3][1][1] == 200 and line[3][1][2] == 0)
assert(body[2][1][2] == 64 and body[2][3][2] == 1)
assert(#G.MakeVertices({}, 200, 64) == 0)
assert(#G.MakeVertices({1}, 200, 64) == 0)
assert(G.ClampLife(0/0) == 0 and G.ClampLife(math.huge) == 0)

-- Stub only the actor interfaces used by the actual widget.
local concat = {__concat = function(a, b) for k, v in pairs(b) do a[k] = v end return a end}
Def = {ActorFrame = function(t) return t end, ActorMultiVertex = function(t) return t end}
LoadActor = function() return setmetatable({}, concat) end
LoadFont = LoadActor
ToEnumShortString = function(p) return p:sub(-2) end
SCREEN_LEFT, SCREEN_RIGHT, SCREEN_BOTTOM = 0, 1280, 720
SCREEN_CENTER_X, SCREEN_CENTER_Y = 640, 360
THEME = {
	GetMetric = function(_, _, key) return ({Width=400, Height=128, MarginLeft=96, MarginBottom=71})[key] end,
	GetString = function() return 'LIFE' end,
	GetPathG = function(_, group, element) return group .. ' ' .. element end,
}
local function Stage(seconds, tail, life)
	local stats = {tail=tail, life=life, samples=0}
	function stats:GetLifeRecordLastSecond() return self.tail end
	function stats:GetCurrentLife() return self.life end
	function stats:GetLifeRecord(last, count)
		self.samples, self.last = self.samples + 1, last
		assert(count == 128)
		local values = {}
		for i=1,count do values[i] = self.life end
		return values
	end
	local stage = {seconds=seconds, stats=stats}
	function stage:GetStepsSeconds() return self.seconds end
	function stage:GetPlayerStageStats(p) assert(p == humans[1]) return self.stats end
	return stage
end
local current, evaluated = Stage(10, 8, 0.75), Stage(30, 33, 0.25)
STATSMAN = {GetCurStageStats = function() return current end}
SCREENMAN = {GetTopScreen = function() return {
	GetStageStats = function() return evaluated end,
} end}
assert(G.GetEndSecond(current, current.stats) == 10)
assert(G.GetEndSecond(evaluated, evaluated.stats) == 33)
local function Instantiate(def)
	local actor = {def=def, children={}}
	for _, name in ipairs({'xy','x','halign','valign','zoomto','zoom','shadowlength','stoptweening',
		'diffusealpha','linear','SetDrawState','SetLineWidth','draworder'}) do
		actor[name] = function(self, ...) self[name .. 'Args'] = {...}; return self end
	end
	function actor:visible(v) self.shown = v; return self end
	function actor:settext(text) self.text = text; return self end
	function actor:SetVertices(vertices) self.vertices = vertices; return self end
	function actor:SetUpdateFunction(callback) self.update = callback; return self end
	function actor:GetChild(name) return self.children[name] end
	for _, child in ipairs(def) do
		local c = Instantiate(child)
		if child.Name then actor.children[child.Name] = c end
	end
	if def.InitCommand then def.InitCommand(actor) end
	return actor
end
local function Widget(live)
	local def = assert(loadfile('Themes/default/Graphics/LifeHistory graph.lua'))(live)
	return Instantiate(def), def
end
local actor, def = Widget(true)
assert(actor.xyArgs[1] == 96 and actor.xyArgs[2] == 521)
assert(actor.draworderArgs[1] > 0) -- Above the underlay's screen filter.
assert(actor:GetChild('Backing').diffusealphaArgs[1] == 0.8)
assert(actor:GetChild('Body').diffusealphaArgs[1] == 0.9)
def.OnCommand(actor)
assert(actor.update and current.stats.last == 10 and evaluated.stats.samples == 0)
assert(actor.diffusealphaArgs[1] == 1) -- Preserve existing opacity.
assert(actor:GetChild('Value').text == '75%')
assert(actor:GetChild('EndTime').text == '0:10')
local sampled = current.stats.samples
actor.update(actor) -- Unchanged/paused state must not resample.
assert(current.stats.samples == sampled)
current.seconds = 10.1
actor.update(actor)
assert(current.stats.samples == sampled + 1 and current.stats.last == 10.1)
current.stats.life = 0.4 -- A life change at the same time updates immediately.
def.LifeMeterChangedP1MessageCommand(actor)
assert(actor:GetChild('Value').text == '40%')
assert(math.abs(actor:GetChild('Line').vertices[128][1][2] - 76.8) < 0.001)
for i=1,1000 do
	current.seconds = 10.1 + i / 60
	actor.update(actor)
end
assert(#actor:GetChild('Body').vertices == 256 and #actor:GetChild('Line').vertices == 128)
enabled = {'PlayerNumber_P1', 'PlayerNumber_P2'}
actor.update(actor)
assert(actor.shown == false)
enabled = humans
actor.update(actor)
assert(actor.shown == true)
current = Stage(0, 0, 0.5) -- A fresh stage has no previous plot state.
def.DoneLoadingNextSongMessageCommand(actor)
assert(actor:GetChild('EndTime').text == '0:00' and actor:GetChild('Value').text == '50%')
def.OffCommand(actor)
sampled = current.stats.samples
def.LifeMeterChangedP1MessageCommand(actor)
assert(actor.update == nil and current.stats.samples == sampled)

-- Evaluation must read its own selected (possibly summary) statistics.
actor, def = Widget(false)
def.OnCommand(actor)
assert(actor.update == nil and evaluated.stats.last == 33)
assert(actor.draworderArgs[1] < 0) -- Below the evaluation statistics.
assert(actor:GetChild('Value').text == '25%' and actor:GetChild('EndTime').text == '0:33')
sampled = evaluated.stats.samples
def.LifeMeterChangedP1MessageCommand(actor)
assert(evaluated.stats.samples == sampled)
-- P2 uses its own message; two-player screens get no graph actor.
humans, enabled = {'PlayerNumber_P2'}, {'PlayerNumber_P2'}
actor, def = Widget(true)
assert(def.LifeMeterChangedP2MessageCommand and not def.LifeMeterChangedP1MessageCommand)
def.OnCommand(actor)
assert(actor:GetChild('Value').text == '50%')
-- Both single-player sides retain the full graph beside the life-bar frame,
-- even on 4:3 screens; notes are in a higher screen layer.
for _, p in ipairs({'PlayerNumber_P1', 'PlayerNumber_P2'}) do
	humans, enabled = {p}, {p}
	for _, screenWidth in ipairs({960, 1280}) do
		SCREEN_RIGHT, SCREEN_CENTER_X = screenWidth, screenWidth / 2
		actor, def = Widget(true)
		def.OnCommand(actor)
		assert(actor.xyArgs[1] >= SCREEN_LEFT + 40 + 64 / 2 + 24)
		assert(actor:GetChild('Backing').zoomtoArgs[1] == 400)
		assert(actor:GetChild('Line').vertices[128][1][1] == 400)
		assert(actor.xyArgs[2] + 128 == SCREEN_BOTTOM - 71)
	end
end
actor, def = Widget(false)
def.OnCommand(actor)
assert(actor:GetChild('Backing').zoomtoArgs[1] == 400)
-- The last evaluation row and song title remain clear of the left-side graph.
for _, screenWidth in ipairs({960, 1280}) do
	SCREEN_RIGHT, SCREEN_CENTER_X = screenWidth, screenWidth / 2
	for _, rows in ipairs({7, 8}) do
		local layout = G.GetEvaluationLayout(rows)
		local rowBottom = SCREEN_CENTER_Y / 1.48 + rows * layout.spacing + 18 * layout.scale
		assert(rowBottom < 521 - 24)
		assert(layout.titleX - layout.titleWidth / 2 >= 96 + 400 + 12)
		assert(layout.titleX + layout.titleWidth / 2 <= SCREEN_CENTER_X + 320 - 95 - 12)
	end
end
SCREEN_RIGHT, SCREEN_CENTER_X = 1920, 960
local layout = G.GetEvaluationLayout(8)
assert(layout.spacing == 38 and layout.scale == 1 and layout.titleX == 960)
humans, enabled = {'PlayerNumber_P1', 'PlayerNumber_P2'}, {'PlayerNumber_P1', 'PlayerNumber_P2'}
actor, def = Widget(true)
assert(#def == 0 and def.OnCommand == nil)
SCREEN_RIGHT, SCREEN_CENTER_X = 960, 480
layout = G.GetEvaluationLayout(8)
assert(layout.spacing == 38 and layout.scale == 1 and layout.titleX == 480)
print('Life-history eligibility, geometry, live updates, cleanup, and evaluation checks passed.')
