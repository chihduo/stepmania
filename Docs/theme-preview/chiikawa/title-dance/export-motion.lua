-- Run from repository root with the bundled Lua interpreter.
GetTimeSinceStart=function() return 0 end
dofile("Themes/chiikawa/Scripts/06 Chiikawa.lua")
dofile("Themes/chiikawa/Scripts/07 TitleDance.lua")
local C=Chiikawa;local c=C.TitleDanceConfig;local fps=30
io.write(string.format('window.titleDanceData={"duration":%.9g,"fps":%d,"width":%d,"height":%d,"spriteSize":%d,"feetY":%.9g,"footAnchor":%.9g,"padsY":%.9g,"padsWidth":%d,"padsHeight":%d,"centers":[%g,%g,%g],"frames":[',c.duration,fps,c.width,c.height,c.spriteSize,c.feetY,c.footAnchor,c.padsY,c.padsWidth,c.padsHeight,unpack(c.centers)))
for frame=0,math.floor(c.duration*fps+.5)-1 do
    if frame>0 then io.write(',') end
    io.write('[')
    for character=1,3 do
        if character>1 then io.write(',') end
        local p=C.TitleDancePose(frame/fps,character)
        io.write(string.format('{"state":%d,"x":%.9g,"y":%.9g,"angle":%.9g}',p.state,p.x,p.y,p.angle))
    end
    io.write(']')
end
io.write(']};\n')
