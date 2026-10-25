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
SCREEN_RIGHT, SCREEN_BOTTOM = 1280, 720
THEME = {
	GetMetric = function(_, _, key) return ({Width=200, Height=64, MarginRight=16, MarginBottom=56})[key] end,
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
local field
GAMESTATE.GetCurrentStyle = function() return {GetWidth = function() return 256 end} end
SCREENMAN = {GetTopScreen = function() return {
	GetStageStats = function() return evaluated end,
	GetChild = function() return field end,
} end}
assert(G.GetEndSecond(current, current.stats) == 10)
assert(G.GetEndSecond(evaluated, evaluated.stats) == 33)
local function Instantiate(def)
	local actor = {def=def, children={}}
	for _, name in ipairs({'xy','x','halign','valign','zoomto','zoom','shadowlength','stoptweening',
		'diffusealpha','linear','SetDrawState','SetLineWidth'}) do
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
assert(actor.xyArgs[1] == 1064 and actor.xyArgs[2] == 600)
def.OnCommand(actor)
assert(actor.update and current.stats.last == 10 and evaluated.stats.samples == 0)
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
assert(math.abs(actor:GetChild('Line').vertices[128][1][2] - 38.4) < 0.001)
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
-- At 4:3, the P2 note lane can leave only a narrow corner for the plot.
SCREEN_RIGHT = 960
field = {GetX = function() return 688 end, GetZoomX = function() return 1.5 end}
actor, def = Widget(true)
def.OnCommand(actor)
assert(actor.xyArgs[1] == 892)
assert(actor:GetChild('Backing').zoomtoArgs[1] == 52)
assert(actor:GetChild('Line').vertices[128][1][1] == 52)
-- Evaluation retains the normal width because it has no note lane.
actor, def = Widget(false)
def.OnCommand(actor)
assert(actor:GetChild('Backing').zoomtoArgs[1] == 200)
humans, enabled = {'PlayerNumber_P1', 'PlayerNumber_P2'}, {'PlayerNumber_P1', 'PlayerNumber_P2'}
actor, def = Widget(true)
assert(#def == 0 and def.OnCommand == nil)
print('Life-history eligibility, geometry, live updates, cleanup, and evaluation checks passed.')
