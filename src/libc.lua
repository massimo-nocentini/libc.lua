
local liblibc = require 'liblibc'

local libc = {}

libc.stdlib = {
	l64a = liblibc.l64a,
	a64l = liblibc.a64l,
	lldiv = liblibc.lldiv,
}

libc.string = {
	strcmp = liblibc.strcmp,
}

function libc.stdlib.lteqgtcmp (a, b)  
	if a < b then return -1
	elseif a == b then return 0
	else return 1 end
end

function libc.stdlib.qsort (tbl, compare)
	return liblibc.qsort(tbl, compare or libc.stdlib.lteqgtcmp) end

function libc.stdlib.bsearch (tbl, key, compare)
	return liblibc.bsearch(tbl, key, compare or libc.stdlib.lteqgtcmp) end

libc.math = {
	fma = liblibc.fma,
}

liblibc.constants (libc.math)	-- augment the `math` table with some constants

return libc
