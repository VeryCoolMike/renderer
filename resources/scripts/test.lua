
-- THIS IS SOME SAMPLE CODE

--[[
local currentPosY = 0.0

while true do wait(0.5)
    local part = Instance.new("Object")
    currentPosY = currentPosY + 2
    part.pos = getCameraPos()
    print(part.pos[2])
    print(part.name);
end
]]--