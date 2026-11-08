-- Run from the repository root using the bundled Lua interpreter.
GetTimeSinceStart=function() return 12 end
math.random=function() error("Title animation consumed gameplay RNG") end
math.randomseed=function() error("Title animation reseeded gameplay RNG") end
dofile("Themes/chiikawa/Scripts/06 Chiikawa.lua")
dofile("Themes/chiikawa/Scripts/07 TitleDance.lua")
local C=Chiikawa
local period=C.TitleDanceConfig.duration
local seen={}
for character=1,3 do
    seen[character]={}
    for frame=0,319 do
        local time=frame*period/320
        local a,b=C.TitleDancePose(time,character),C.TitleDancePose(time+period,character)
        assert(a.state==b.state)
        assert(a.state >= (character-1)*4 and a.state < character*4)
        for _,field in ipairs({"x","y","angle"}) do assert(math.abs(a[field]-b[field])<1e-8) end
        assert(math.abs(a.x)<=28 and a.y>=-34 and a.y<=0)
        seen[character][a.state]=true
    end
    for state=(character-1)*4,character*4-1 do assert(seen[character][state]) end
    for boundary=0,4 do
        local t=boundary*period/4
        local a,b=C.TitleDancePose(t-1e-7,character),C.TitleDancePose(t+1e-7,character)
        assert(a.state==b.state, "Pose discontinuity at section/loop boundary")
        for _,field in ipairs({"x","y","angle"}) do assert(math.abs(a[field]-b[field])<1e-8) end
    end
end
for _,path in ipairs({C.TitleDanceConfig.dancers,C.TitleDanceConfig.pads}) do
    local file=assert(io.open(path:sub(2),"rb"));file:close()
end
local moving=true
ThemePrefs={Get=function() return moving end}
Def={ActorFrame=function(t) return t end,Sprite=function(t) return t end}
local function actor()
    local a={}
    for _,method in ipairs({"visible","zoom","setstate","rotationz","animate"}) do
        local name=method;a[name]=function(self,value) self[name.."Value"]=value;return self end
    end
    function a:xy(x,y) self.x,self.y=x,y;return self end
    function a:zoomto(w,h) self.w,self.h=w,h;return self end
    function a:SetUpdateFunction(fn) self.update=fn;return self end
    function a:GetChild(name) return self.children[name] end
    return a
end
for _,width in ipairs({960,1280}) do
    SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_CENTER_X,SCREEN_CENTER_Y=width,720,width/2,360
    local spec=C.TitleDance();local root=actor();root.children={}
    for _,child in ipairs(spec) do
        local a=actor();root.children[child.Name]=a;child.InitCommand(a)
    end
    spec.InitCommand(root);assert(root.visibleValue==false)
    spec.OnCommand(root);assert(root.visibleValue and root.update)
    local box=C.Layout("title")
    assert(root.x==box.x and root.y==box.y)
    assert(root.zoomValue*C.TitleDanceConfig.width<=box.width+1e-8)
    assert(root.zoomValue*C.TitleDanceConfig.height<=box.height+1e-8)
    local pads=root:GetChild("DancePads");local px,py=pads.x,pads.y
    root.update(root,period/8)
    assert(pads.x==px and pads.y==py,"Pads must remain stationary")
    assert(root:GetChild("Dancer1").setstateValue==0)
    spec.OffCommand(root);assert(root.update==nil)
    spec.OnCommand(root);assert(root:GetChild("Dancer1").setstateValue==1)
    moving=false;spec.OnCommand(root);assert(root.update==nil)
    for i=1,3 do
        local a=root:GetChild("Dancer"..i)
        assert(a.setstateValue==(i-1)*4+1 and a.rotationzValue==0)
        assert(a.x==C.TitleDanceConfig.centers[i])
    end
    moving=true;spec.OnCommand(root);assert(root.update)
end
print("Title dance: periodic motion/poses, stationary pads, RNG isolation, 16:9/4:3 sizing, off/return lifecycle and motion-off checks passed")
