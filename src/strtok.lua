
local libc = require 'libc'

print [[
    hello
]]

local str = [[

this is
a splitted
string, over

lines


]]

print (str)

local lines = libc.string.strtok_r (
    str,
    '\n',
    true
)

for k, v in pairs (lines) do print (k, v) end

print '-------------------------'

for k, v in pairs (str:lines (false)) do print (k, v) end

lines = libc.string.strtok_r ('hello world', '\r\n,')

for k, v in pairs (lines) do print (k, v) end
