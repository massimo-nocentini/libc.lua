
local libc = require 'libc'

print [[ 
    hello]]

local str = [[ 
 
this is
a splitted
string, over
 
lines
 ]]

local lines = libc.string.strtok (
    str,
    '\n'
)

for k, v in pairs (lines) do print (k, v) end

lines = libc.string.strtok ('hello world', '\r\n,')

for k, v in pairs (lines) do print (k, v) end

for line in str:gmatch("([^\n]*)\n?") do
    print (line)
  end