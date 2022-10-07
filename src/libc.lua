
local liblibc = require 'liblibc'

local libc = {}

setmetatable(libc, { __index = liblibc })

function libc.qsort (tbl, compare)

	local function C (a, b)  
		if a < b then return -1
		elseif a == b then return 0
		else return 1 end
	end

	compare = compare or C
	return liblibc.qsort(tbl, compare)
end

return libc
