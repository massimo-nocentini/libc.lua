
local libc = require 'libc'
local lambda = require 'operator'

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

local lines = libc.string.strtok_r (str, '\n', true)

for k, v in pairs (lines) do print (k, v) end

for k, v in pairs (str:lines (false)) do print (k, v) end

lines = libc.string.strtok_r ('hello world', '\r\n,')

for k, v in pairs (string.lines 'hello world') do print (k, v) end

local rows = table.scan (str:lines (true), function (acc, l) return 1 + acc + #l end, 0)

for k, v in ipairs (rows) do print (v) end

local function I (row, col)
    return string.sub (str, rows[row] + col)
end

print ('indexed ', I (2, 4)  )

print (#rows, #str)

for k, v in pairs (string.tokenize ('hello, world', ',')) do print (k, v) end