-- Shared life-history sampling and geometry for gameplay and evaluation.
LifeHistoryGraph = {}
LifeHistoryGraph.SampleCount = 128

local function finite(value)
	value = tonumber(value) or 0
	if value ~= value or math.abs(value) == math.huge then return 0 end
	return value
end

function LifeHistoryGraph.ClampLife(value)
	return math.max(0, math.min(1, finite(value)))
end

function LifeHistoryGraph.GetPlayer()
	local humans = GAMESTATE:GetHumanPlayers()
	if #humans ~= 1 or #GAMESTATE:GetEnabledPlayers() ~= 1
		or GAMESTATE:IsDemonstration() then return nil end
	-- The engine deliberately does not retain life history in endless courses.
	local course = GAMESTATE:GetCurrentCourse()
	if course and course:IsEndless() then return nil end
	return humans[1]
end

function LifeHistoryGraph.GetEndSecond(stage, player)
	-- Life records use elapsed play time, not song seconds or the current rate.
	-- Failure markers and summary gaps can extend past the stage's step clock.
	return math.max(0, finite(stage:GetStepsSeconds()),
		finite(player:GetLifeRecordLastSecond()))
end

function LifeHistoryGraph.MakeVertices(values, width, height)
	local body, line = {}, {}
	width, height = finite(width), finite(height)
	if #values < 2 or width <= 0 or height <= 0 then return body, line end
	local white = {1, 1, 1, 1}
	for i, value in ipairs(values) do
		local u = (i - 1) / (#values - 1)
		local v = 1 - LifeHistoryGraph.ClampLife(value)
		local x, y = u * width, v * height
		body[#body + 1] = {{x, y, 0}, white, {u, v}}
		body[#body + 1] = {{x, height, 0}, white, {u, 1}}
		line[#line + 1] = {{x, y, 0}, white}
	end
	return body, line
end

-- Keep evaluation text above/beside the enlarged graph on narrow screens.
function LifeHistoryGraph.GetEvaluationLayout(rows)
	local layout = {spacing = 38, scale = 1, titleX = SCREEN_CENTER_X, titleWidth = 375}
	if not LifeHistoryGraph.GetPlayer() then return layout end
	local graphLeft = SCREEN_RIGHT - THEME:GetMetric("LifeHistory", "MarginRight")
		- THEME:GetMetric("LifeHistory", "Width")
	local graphTop = SCREEN_BOTTOM - THEME:GetMetric("LifeHistory", "MarginBottom")
		- THEME:GetMetric("LifeHistory", "Height")
	if graphLeft < SCREEN_CENTER_X + 200 then
		layout.spacing = math.min(38, math.max(1,
			math.floor((graphTop - 40 - SCREEN_CENTER_Y / 1.48) / rows)))
		layout.scale = layout.spacing / 38
	end
	local titleRight = graphLeft - 12
	if titleRight < SCREEN_CENTER_X + layout.titleWidth / 2 then
		-- The single-player radar/details column occupies center - 320 +/- 95.
		local titleLeft = SCREEN_CENTER_X - 320 + 95 + 12
		layout.titleX = (titleLeft + titleRight) / 2
		layout.titleWidth = math.max(1, titleRight - titleLeft)
	end
	return layout
end
