local l = file.list()
for k,v in pairs(l) do
    --node.compile('system.lua')
    if string.match(k, ".(lua)") ~= nil then
        if k ~= 'init.lua' then
            print("compile name :"..k..", size size:"..v)
            node.compile(k)
            --print("compile name :"..k.." complete remove this")
            --file.remove(k)
        end

    end
end
--[[
print('compile system.lua')
node.compile('system.lua')

print('compile wifi_config.lua')
node.compile('wifi_config.lua')

print('compile esutil.lua')
node.compile('esutil.lua')
]]--