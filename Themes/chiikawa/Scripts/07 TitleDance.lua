-- The title illustration uses PNG sprite poses: the engine only reads frame 1
-- of a GIF. All positions come from a periodic clock; pads remain stationary.
local C = Chiikawa
C.TitleDanceConfig = {
    duration=3.2,
    width=660, height=480,
    spriteSize=320,
    centers={-211, 0, 211},
    feetY=104,
    footAnchor=374/416,
    padsY=105, padsWidth=660, padsHeight=220,
    dancers=C.ArtPath .. "title-dancers 4x3.png",
    pads=C.ArtPath .. "title-dance-pads.png",
}

-- State is zero-based within the four poses for one character:
-- left step, neutral, right step, hop. Each character is a row in the atlas.
function C.TitleDancePose(seconds, character)
    -- Round below pixel precision so exact pose boundaries survive wraparound.
    local phase = (seconds / C.TitleDanceConfig.duration + (character-1)/4) % 1
    phase = (math.floor(phase*1000000000+.5) % 1000000000)/1000000000
    local section = math.floor(phase*4)
    local u = phase*4-section
    local wave = math.sin(math.pi*u)^2
    local pose, x, y, angle = 1, 0, 0, 0
    if section == 0 or section == 1 then
        local direction = section == 0 and -1 or 1
        x, y, angle = direction*28*wave, -4*wave, direction*4*wave
        if u > .15 and u < .85 then pose = section == 0 and 0 or 2 end
    elseif section == 2 then
        y = -34*wave
        angle = 3*math.sin(2*math.pi*u)*wave
        if u > .15 and u < .85 then pose = 3 end
    else
        x = 10*math.sin(2*math.pi*u)*wave
        y = -9*math.sin(2*math.pi*u)^2
        if u > .12 and u < .44 then pose = 0 end
        if u > .56 and u < .88 then pose = 2 end
        angle = -3*math.sin(2*math.pi*u)*wave
    end
    return {state=(character-1)*4+pose, x=x, y=y, angle=angle}
end

function C.TitleDance()
    local config = C.TitleDanceConfig
    local elapsed = 0
    local function draw(self, seconds, moving)
        for i=1,3 do
            local pose = moving and C.TitleDancePose(seconds, i)
                or {state=(i-1)*4+1, x=0, y=0, angle=0}
            self:GetChild("Dancer" .. i):setstate(pose.state)
                :xy(config.centers[i]+pose.x,
                    config.feetY-config.spriteSize*(config.footAnchor-.5)+pose.y)
                :rotationz(pose.angle)
        end
    end
    local t = Def.ActorFrame {
        Name="ChiikawaTitleDance",
        InitCommand=function(self) self:visible(false) end,
        OnCommand=function(self)
            local box = C.Layout("title")
            elapsed = 0
            self:visible(true):xy(box.x,box.y)
                :zoom(math.min(box.width/config.width,box.height/config.height))
            local moving = ThemePrefs.Get("FancyUIBG")
            draw(self,0,moving)
            if moving then
                self:SetUpdateFunction(function(actor, delta)
                    elapsed = (elapsed+delta) % config.duration
                    draw(actor,elapsed,true)
                end)
            else
                self:SetUpdateFunction(nil)
            end
        end,
        OffCommand=function(self) self:SetUpdateFunction(nil) end,
        Def.Sprite {
            Name="DancePads",
            Texture=config.pads,
            InitCommand=function(self)
                self:xy(0,config.padsY):zoomto(config.padsWidth,config.padsHeight)
            end,
        },
    }
    for i=1,3 do
        local character = i
        t[#t+1] = Def.Sprite {
            Name="Dancer" .. character,
            Texture=config.dancers,
            InitCommand=function(self)
                self:animate(false):zoomto(config.spriteSize,config.spriteSize)
                    :setstate((character-1)*4+1)
            end,
        }
    end
    return t
end
