
local libc = require 'libc'

assert (libc.l64a(32), 'U')
assert (libc.l64a(329547523879582), 'S8D411')

assert (libc.a64l(libc.l64a(32)), 32)
assert (libc.a64l(libc.l64a(329547523879582)), 329547523879582)

local q, r = libc.lldiv(-5, 3)
assert (q == -1 and r == -2)

------------------------------------------------------------------

local values, key = { 50, 20, 60, 40, 10, 30 }, 40

libc.qsort(values, libc.lteqgtcmp)
local found = libc.bsearch(values, key, libc.lteqgtcmp)
assert (found == key)

------------------------------------------------------------------

values, key = {"some","example","strings","here"}, "example"

libc.qsort(values, libc.strcmp)
found = libc.bsearch(values, key, libc.strcmp)
assert (found == key)