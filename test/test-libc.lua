
local lu = require 'luaunit'
local libc = require 'libc'

function test_l64a ()
	lu.assertEquals (libc.l64a(32), 'U')
	lu.assertEquals (libc.l64a(329547523879582), 'S8D411')
end

function test_a64l ()
    lu.assertEquals (libc.a64l(libc.l64a(32)), 32)
    lu.assertEquals (libc.a64l(libc.l64a(329547523879582)), 3273192094)
end

function test_lldiv ()
    local q, r = libc.lldiv(-5, 3)
    lu.assertEquals (q, -1)
    lu.assertEquals (r, -2)
end

function test_bsearch_int ()
    local values, key = { 50, 20, 60, 40, 10, 30 }, 40
    libc.qsort(values)
    local found = libc.bsearch(values, key)
    lu.assertEquals (found, key)
end

function test_bsearch_str ()
    values, key = {"some","example","strings","here"}, "example"
    libc.qsort(values, libc.strcmp)
    found = libc.bsearch(values, key, libc.strcmp)
    lu.assertEquals (found, key)
end

os.exit( lu.LuaUnit.run() )
