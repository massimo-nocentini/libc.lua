
local lu = require 'luaunit'
local libc = require 'libc'

Test_base64 = {}

function Test_base64:test_l64a ()
	lu.assertEquals (libc.l64a(32), 'U')
	lu.assertEquals (libc.l64a(23948792), 'srKP/')
	lu.assertEquals (libc.l64a(0), '')
end

function Test_base64:test_a64l ()
    lu.assertEquals (libc.a64l('U'), 32)
    lu.assertEquals (libc.a64l('srKP/'), 23948792)
end

function Test_base64:test_a64l_l64a_identity ()
    lu.assertEquals (libc.a64l(libc.l64a(32)), 32)
    lu.assertEquals (libc.a64l(libc.l64a(23948792)), 23948792)
end

--------------------------------------------------------------------------------

Test_lldiv = {}

function Test_lldiv:test_minus5_3 ()
    local q, r = libc.lldiv(-5, 3)
    lu.assertEquals (q, -1)
    lu.assertEquals (r, -2)
end

function Test_lldiv:test_4897294869528760942_759843276952576 ()
    local m, n = 4897294869528760942, 759843276952576
    local q, r = libc.lldiv(m, n)
    lu.assertEquals (q, 6445)
    lu.assertEquals (r, 104949569408622)
    lu.assertEquals (q * n + r, m)
end

--------------------------------------------------------------------------------

Test_bsearch = {}

function Test_bsearch:test_lteqgtcmp ()
    local values, key = { 50, 20, 60, 40, 10, 30 }, 40
    libc.qsort(values)
    local found = libc.bsearch(values, key)
    lu.assertEquals (found, key)
end

function Test_bsearch:test_strcmp ()
    local values, key = {"some","example","strings","here"}, "example"
    libc.qsort(values, libc.strcmp)
    local found = libc.bsearch(values, key, libc.strcmp)
    lu.assertEquals (found, key)
end

--------------------------------------------------------------------------------

Test_qsort = {}

function Test_qsort:test_1e6 ()
	
	local tbl = {}
	local n = 1000000 

	for i=1,n do table.insert(tbl, math.random(n)) end	-- prepare the input.

	local sorted, perm = libc.qsort(tbl)

	local permuted = {}
	for i, p in ipairs(perm) do permuted[i] = tbl[p] end

	table.sort(tbl)

	lu.assertEquals(sorted, tbl)
	lu.assertEquals(permuted, tbl)
end

--------------------------------------------------------------------------------
Test_fma = {}

function Test_fma:test_10_20_30 ()
	
	local x, y, z = 10.0, 20.0, 30.0
	lu.assertEquals(libc.fma(x, y, z), x * y + z)
end


function Test_fma:test_a_b_minus ()
	
	local x, y = 1.0, 0.000001
    local high = x * y
    local low = libc.fma(x, y, -high)
	lu.assertEquals(high + low, x * y)
    lu.assertEquals(libc.fma (x, y, 0.0), x * y)
end

--------------------------------------------------------------------------------

os.exit( lu.LuaUnit.run() )
