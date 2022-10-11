
local liblibc = require 'liblibc'

local libc = {}

setmetatable(libc, { __index = liblibc })

function libc.lteqgtcmp (a, b)  
	if a < b then return -1
	elseif a == b then return 0
	else return 1 end
end

function libc.qsort (tbl, compare)

	compare = compare or libc.lteqgtcmp
	return liblibc.qsort(tbl, compare)
end

function libc.bsearch (tbl, key, compare)

	compare = compare or libc.lteqgtcmp
	return liblibc.bsearch(tbl, key, compare)
end

return libc
