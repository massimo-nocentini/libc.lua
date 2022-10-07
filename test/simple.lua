
local libc = require 'libc'

local tbl = {}
local n = 20

for i=1,n do
    table.insert(tbl, math.random(n))
end

print(table.concat(tbl, ', '))

local sorted, perm = libc.qsort(tbl)

print(table.concat(sorted, ', '))
print(table.concat(perm, ', '))

