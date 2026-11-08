-- Run from the repository root with the bundled Lua 5.1 interpreter.
-- Exercise the real background-effect callbacks without a renderer.
local poster = "/Themes/test/Graphics/Common fallback background.png"
local movie = "/Themes/test/Movies/fallback.mp4"
local file, metric, present, static, backgrounds = poster, movie, true, false, true
local metricAvailable = true
Var = function(name) if name == "File1" then return file else return "white" end end
color = function(value) return value end
THEME = {
    HasMetric=function() return metricAvailable end,
    GetMetric=function() return metric end,
    GetPathG=function() return poster end,
}
PREFSMAN = {GetPreference=function() return backgrounds end}
GAMESTATE = {GetSongOptionsObject=function()
    return {StaticBackground=function() return static end}
end}
FILEMAN = {DoesFileExist=function(_, path) return present and path == movie end}
Def = {ActorFrame=function(t) return t end}
LoadActor = function(path)
    return setmetatable({path=path}, {__concat=function(a,b) b.path=a.path;return b end})
end
SCREEN_CENTER_X,SCREEN_CENTER_Y=640,360
local function effect() return dofile("BackgroundEffects/StretchNormal.lua")[1] end

assert(effect().path == movie, "No-art song should use the configured movie")
file="/Songs/Pack/Song/background.png";assert(effect().path == file)
file="/Songs/Pack/Song/scripted.mp4";assert(effect().path == file)
file=poster
static=true;assert(effect().path == poster);static=false
backgrounds=false;assert(effect().path == poster);backgrounds=true
present=false;assert(effect().path == poster);present=true
metric="";assert(effect().path == poster)
metric=false;assert(effect().path == poster)
metric=movie
metricAvailable=false;assert(effect().path == poster);metricAvailable=true

local texture={rate=function(self,rate) self.value=rate end}
local parent={GetUpdateRate=function() return 1.25 end}
local actor={}
function actor:xy(x,y) self.x,self.y=x,y;return self end
function actor:scale_or_crop_background() self.scaled=true;return self end
function actor:diffuse(value) self.color=value;return self end
function actor:effectclock(value) self.clock=value;return self end
function actor:GetTexture() return texture end
function actor:GetParent() return parent end
function actor:play() self.playing=true end
function actor:pause() self.playing=false end
local spec=effect()
spec.OnCommand(actor)
assert(actor.x==640 and actor.y==360 and actor.scaled and actor.color=="white")
assert(actor.clock=="music" and texture.value==1.25)
spec.GainFocusCommand(actor);assert(actor.playing)
spec.LoseFocusCommand(actor);assert(not actor.playing)
print("Fallback movie: selection, static/disabled/missing-file fallback, music clock and focus handlers passed")
