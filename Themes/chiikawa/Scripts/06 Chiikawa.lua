-- Chiikawa decorates the default theme; fonts, notes and judgments are inherited.
Chiikawa = {}
local C = Chiikawa
C.ArtPath = "/Themes/chiikawa/Art/"
C.Pools = {
	title = {"12-acoustic-duet", "01-character-trio", "11-listening-together", "14-piano-duet", "15-drum-circle", "16-violin-serenade", "17-flute-practice", "18-saxophone-swing"},
	select = {"11-listening-together", "12-acoustic-duet", "01-character-trio", "14-piano-duet", "15-drum-circle", "16-violin-serenade", "17-flute-practice", "18-saxophone-swing"},
	game = {"10-rest", "11-listening-together", "14-piano-duet", "16-violin-serenade", "17-flute-practice"},
	result = {"04-results-replay", "13-shared-jump", "15-drum-circle", "18-saxophone-swing"},
	replay = {"10-rest", "11-listening-together", "12-acoustic-duet", "14-piano-duet", "16-violin-serenade", "17-flute-practice"},
}

-- A private Park-Miller stream: decoration never advances/reseeds math.random,
-- which the engine and noteskins may use for gameplay or replay presentation.
function C.NewSelector(seed)
	local state = math.floor(seed or 1) % 2147483647
	if state < 1 then state = 1 end
	-- Mix nearby startup-time seeds before the first screen selection.
	for i=1,4 do state = (state * 16807) % 2147483647 end
	local previous = {}
	return function(role, pool)
		local choices = {}
		for _, name in ipairs(pool) do
			if name ~= previous[role] then choices[#choices + 1] = name end
		end
		if #choices == 0 then choices = pool end
		if #choices == 0 then return nil end
		state = (state * 16807) % 2147483647
		local name = choices[math.floor((state - 1) / 2147483646 * #choices) + 1]
		previous[role] = name
		return name
	end
end
local choose = C.NewSelector(GetTimeSinceStart() * 1000000)

local function singlePlayer()
	local humans = GAMESTATE:GetHumanPlayers()
	if #humans ~= 1 or #GAMESTATE:GetEnabledPlayers() ~= 1
		or GAMESTATE:IsDemonstration() then return nil end
	return humans[1]
end

-- Coordinates use the theme's virtual 720-pixel height at either aspect ratio.
-- Crowded fields deliberately get no character decoration.
function C.Layout(role)
	local w, h, cx, cy = SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_CENTER_X, SCREEN_CENTER_Y
	if role == "title" then return {x=w*.73, y=h*.59, width=w*.43, height=h*.51} end
	if role == "select" then return {x=cx-228, y=cy+85, width=360, height=168} end
	if role == "replay" then return {x=w*.77, y=h*.53, width=w*.32, height=h*.38} end
	local player = singlePlayer()
	if not player then return nil end
	if role == "result" then
		if GAMESTATE:IsCourseMode() then return nil end
		return {x=cx+(player == PLAYER_1 and 320 or -320), y=300, width=230, height=180}
	end
	if role == "game" then
		-- The fallback movie/poster already contains the trio. Do not put a
		-- second character cutout over it, including during course changes.
		local song = GAMESTATE:GetCurrentSong()
		if song and not song:HasBackground() then return nil end
		local style = GAMESTATE:GetCurrentStyle()
		if not style or style:GetStyleType() ~= "StyleType_OnePlayerOneSide"
			or style:ColumnsPerPlayer() > 4 or Center1Player() then return nil end
		return {x=w*(player == PLAYER_1 and .75 or .25), y=280, width=math.min(330,w*.32), height=220}
	end
end

function C.Decoration(role)
	local selected = false
	return Def.Sprite {
		Name="ChiikawaDecoration",
		InitCommand=function(self) self:visible(false) end,
		-- On runs at visible entry, not preloading. It also runs on cached re-entry.
		-- No beat, update, rate-change, pause or wheel-change handler can reroll it.
		OnCommand=function(self)
			selected = false
			local box = C.Layout(role)
			self:visible(box ~= nil)
			if not box then return end
			local name = choose(role, C.Pools[role])
			self:Load(C.ArtPath .. name .. ".png")
			selected = true
			self:scaletofit(-box.width/2,-box.height/2,box.width/2,box.height/2):xy(box.x,box.y)
			Trace("[Chiikawa] " .. role .. " artwork: " .. name)
		end,
		CurrentSongChangedMessageCommand=function(self)
			if role == "game" then
				-- A course may change backgrounds without reopening the screen.
				-- Hide/show the existing choice without choosing new artwork.
				self:visible(selected and C.Layout(role) ~= nil)
			end
		end,
	}
end

function C.MenuBackground()
	local t = Def.ActorFrame {
		Def.Quad {
			InitCommand=function(self)
				self:xy(SCREEN_CENTER_X,SCREEN_CENTER_Y):zoomto(SCREEN_WIDTH,SCREEN_HEIGHT):diffuse(color("#fff8ee"))
			end,
		},
	}
	if not ThemePrefs.Get("FancyUIBG") then return t end
	local shades = {"#f7bbc8", "#cbe7d7", "#bfdcf0"}
	for i, speed in ipairs({4.5, -7.5, 5.5}) do
		t[#t+1] = Def.Sprite {
			Texture=C.ArtPath .. "menu-tunnel.png",
			InitCommand=function(self)
				self:xy(SCREEN_WIDTH*.64,SCREEN_HEIGHT*.52):zoomto(SCREEN_WIDTH*(1.7-i*.32),SCREEN_WIDTH*(1.7-i*.32))
				self:diffuse(color(shades[i])):diffusealpha(.18):rotationz(i*17):spin():effectmagnitude(0,0,speed)
			end,
		}
	end
	for i=1,9 do
		t[#t+1] = Def.Sprite {
			Texture=C.ArtPath .. "menu-particle.png",
			InitCommand=function(self)
				self:xy(SCREEN_WIDTH*((i*37)%100)/100,SCREEN_HEIGHT*((i*23)%100)/100)
				self:zoom(.13+(i%3)*.025):diffuse(color(shades[(i-1)%3+1])):diffusealpha(.35)
				self:bob():effectmagnitude(12,18,0):effectperiod(12+i):effectoffset(i*.13)
			end,
		}
	end
	return t
end
