-- Run from the repository root with Lua 5.1; no renderer or saved data needed.
GetTimeSinceStart = function() return 12.345 end
math.random = function() error("Decoration consumed gameplay RNG") end
math.randomseed = function() error("Decoration reseeded gameplay RNG") end
dofile("Themes/chiikawa/Scripts/06 Chiikawa.lua")
local C = Chiikawa
local select = C.NewSelector(20261108)
local seen, previous = {}, {}
for visit=1,200 do
	for role,pool in pairs(C.Pools) do
		local selected = select(role,pool)
		assert(selected ~= previous[role], role .. " repeated")
		local allowed = false
		for _,name in ipairs(pool) do if selected == name then allowed = true end end
		assert(allowed, role .. " selected outside pool")
		local file = assert(io.open(C.ArtPath:sub(2) .. selected .. ".png", "rb")); file:close()
		seen[selected], previous[role] = true, selected
	end
end
for _,pool in pairs(C.Pools) do for _,name in ipairs(pool) do assert(seen[name]) end end
assert(select("only",{"one"}) == "one" and select("only",{"one"}) == "one")
assert(select("empty",{}) == nil)
local a,b = C.NewSelector(789), C.NewSelector(789)
for i=1,100 do assert(a("title",C.Pools.title) == b("title",C.Pools.title)) end

PLAYER_1,PLAYER_2 = "PlayerNumber_P1","PlayerNumber_P2"
local humans,enabled,demo,centered,course,style,columns = {PLAYER_1},{PLAYER_1},false,false,false,"StyleType_OnePlayerOneSide",4
local hasBackground = true
GAMESTATE = {
	GetCurrentSong=function() return {HasBackground=function() return hasBackground end} end,
	GetHumanPlayers=function() return humans end,
	GetEnabledPlayers=function() return enabled end,
	IsDemonstration=function() return demo end,
	IsCourseMode=function() return course end,
	GetCurrentStyle=function() return {GetStyleType=function() return style end,ColumnsPerPlayer=function() return columns end} end,
}
Center1Player=function() return centered end
for _,width in ipairs({960,1280}) do
	SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_CENTER_X,SCREEN_CENTER_Y=width,720,width/2,360
	for _,player in ipairs({PLAYER_1,PLAYER_2}) do
		humans,enabled={player},{player}
		for role in pairs(C.Pools) do
			local box=assert(C.Layout(role))
			assert(box.x-box.width/2>=0 and box.x+box.width/2<=width)
			assert(box.y-box.height/2>92 and box.y+box.height/2<670)
		end
		local box=C.Layout("game")
		-- Above the graph and clear of the life bar; on the empty side.
		assert(box.y+box.height/2<521 and box.x-box.width/2>70)
		if player==PLAYER_1 then assert(box.x-box.width/2>width/2)
		else assert(box.x+box.width/2<width/2) end
		box=C.Layout("result");assert(box.y+box.height/2<414)
	end
end
humans,enabled={PLAYER_1,PLAYER_2},{PLAYER_1,PLAYER_2}
assert(C.Layout("game")==nil and C.Layout("result")==nil)
humans,enabled={PLAYER_1},{PLAYER_1}
hasBackground=false;assert(C.Layout("game")==nil);hasBackground=true
centered=true;assert(C.Layout("game")==nil);centered=false
style="StyleType_OnePlayerTwoSides";assert(C.Layout("game")==nil);style="StyleType_OnePlayerOneSide"
columns=5;assert(C.Layout("game")==nil);columns=4
demo=true;assert(C.Layout("game")==nil and C.Layout("result")==nil);demo=false
course=true;assert(C.Layout("result")==nil);course=false

-- Real actor callbacks: no preloading choice; re-entry varies, other messages
-- cannot change a texture, and a crowded layout never loads one.
Def={Sprite=function(t) return t end}
Trace=function() end
local spec=C.Decoration("game")
local actor={loads=0}
function actor:visible(value) self.shown=value;return self end
function actor:Load(path) self.loads=self.loads+1;self.path=path;return self end
function actor:scaletofit() return self end
function actor:xy() return self end
spec.InitCommand(actor);assert(actor.loads==0 and not actor.shown)
spec.OnCommand(actor);local first=actor.path;assert(actor.loads==1 and actor.shown)
for _,message in ipairs({"RateChangedMessageCommand","CurrentSongChangedMessageCommand","PauseMessageCommand","UpdateCommand"}) do
	if spec[message] then spec[message](actor) end
end
assert(actor.loads==1 and actor.path==first)
hasBackground=false;spec.CurrentSongChangedMessageCommand(actor)
assert(not actor.shown and actor.loads==1 and actor.path==first)
hasBackground=true;spec.CurrentSongChangedMessageCommand(actor)
assert(actor.shown and actor.loads==1 and actor.path==first)
spec.OnCommand(actor);assert(actor.loads==2 and actor.path~=first)
centered=true;spec.OnCommand(actor);assert(not actor.shown and actor.loads==2)
print("Chiikawa theme: selection, RNG isolation, entry lifecycle and layout checks passed")
