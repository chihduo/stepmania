-- Shared timing feedback state for gameplay and evaluation screens.
-- Only measured tap judgments are included; misses do not have an offset.
TimingStats = TimingStats or {}

TimingStats.RollingWindowSize = 20
TimingStats.EarlyColor = color("#54B9FF")
TimingStats.LateColor = color("#FF8A65")
TimingStats.CenterColor = color("#FFFFFF")

local player_stats = {}

local function player_key(pn)
	return ToEnumShortString(pn)
end

local function new_stats()
	return {
		recent = {},
		recent_sum = 0,
		total_signed = 0,
		total_absolute = 0,
		count = 0,
	}
end

local function get_stats(pn)
	local key = player_key(pn)
	if not player_stats[key] then
		player_stats[key] = new_stats()
	end
	return player_stats[key]
end

function TimingStats.IsEnabled(pn)
	return not GAMESTATE:IsDemonstration()
		and GetUserPrefB("UserPrefProtiming" .. player_key(pn))
end

function TimingStats.Reset(pn)
	player_stats[player_key(pn)] = new_stats()
end

function TimingStats.AddOffset(pn, offset_seconds, early)
	local offset_ms = math.abs(tonumber(offset_seconds) or 0) * 1000
	local signed_ms = early and -offset_ms or offset_ms
	local stats = get_stats(pn)

	stats.recent[#stats.recent + 1] = signed_ms
	stats.recent_sum = stats.recent_sum + signed_ms
	if #stats.recent > TimingStats.RollingWindowSize then
		stats.recent_sum = stats.recent_sum - table.remove(stats.recent, 1)
	end

	stats.total_signed = stats.total_signed + signed_ms
	stats.total_absolute = stats.total_absolute + offset_ms
	stats.count = stats.count + 1
	return signed_ms
end

function TimingStats.GetCount(pn)
	return get_stats(pn).count
end

function TimingStats.GetRollingAverageMs(pn)
	local stats = get_stats(pn)
	if #stats.recent == 0 then return 0 end
	return stats.recent_sum / #stats.recent
end

function TimingStats.GetMeanOffsetMs(pn)
	local stats = get_stats(pn)
	if stats.count == 0 then return 0 end
	return stats.total_signed / stats.count
end

function TimingStats.GetAccuracyPercent(pn)
	local stats = get_stats(pn)
	if stats.count == 0 then return 0 end
	return clamp(100 - stats.total_absolute / stats.count, 0, 100)
end

function TimingStats.GetDirectionColor(offset_ms)
	if offset_ms < -0.5 then return TimingStats.EarlyColor end
	if offset_ms > 0.5 then return TimingStats.LateColor end
	return TimingStats.CenterColor
end

function TimingStats.FormatOffset(offset_ms)
	return string.format("%d ms", math.floor(math.abs(offset_ms) + 0.5))
end

function TimingStats.FormatAverage(offset_ms)
	return "AVG " .. TimingStats.FormatOffset(offset_ms)
end

function TimingStats.FormatAccuracy(accuracy)
	return string.format("ACCURACY %.1f%%", accuracy)
end
