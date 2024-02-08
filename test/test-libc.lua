
local unittest = require 'unittest'
local libc = require 'libc'
local lambda = require 'operator'

local tests = {}

function tests:test_l64a ()
    unittest.assert.equals '' '' (libc.stdlib.l64a(0))
    unittest.assert.equals '' '/' (libc.stdlib.l64a(1))
    unittest.assert.equals '' '0' (libc.stdlib.l64a(2))
    unittest.assert.equals '' '1' (libc.stdlib.l64a(3))
    unittest.assert.equals '' '2' (libc.stdlib.l64a(4))
	unittest.assert.equals '' 'U' (libc.stdlib.l64a(32))
	unittest.assert.equals '' 'srKP/' (libc.stdlib.l64a(23948792))
	
end

function tests:test_a64l ()
    unittest.assert.equals '' (32) (libc.stdlib.a64l('U'))
    unittest.assert.equals '' (23948792) (libc.stdlib.a64l('srKP/'))
    unittest.assert.equals '' (1296366029) (libc.stdlib.a64l('BbDFBlh5MUo'))
    unittest.assert.equals '' (2607348173) (libc.stdlib.a64l('BbDOPio5MVA'))
    unittest.assert.equals '' (828963277) (libc.stdlib.a64l('BbDOlgL5MVE'))
    unittest.assert.equals '' (3383513544) (libc.stdlib.a64l('653f71ff43be9f0147c54148'))
end

function tests:test_a64l_l64a_identity ()
    local v

    v = 32; unittest.assert.equals '' (v) (libc.stdlib.a64l(libc.stdlib.l64a(v)))
    v = 23948792; unittest.assert.equals '' (v) (libc.stdlib.a64l(libc.stdlib.l64a(v)))
end

function tests:test_minus5_3 ()
    unittest.assert.equals '' (-1, -2) (libc.stdlib.lldiv(-5, 3))
end

function tests:test_4897294869528760942_759843276952576 ()
    local m, n = 4897294869528760942, 759843276952576
    local q, r = libc.stdlib.lldiv(m, n)
    unittest.assert.equals '' (6445, 104949569408622) (q, r)
    unittest.assert.equals '' (m) (q * n + r)
end

function tests:test_lteqgtcmp ()
    local values, key = { 50, 20, 60, 40, 10, 30 }, 40
    libc.stdlib.qsort(values)
    local found = libc.stdlib.bsearch(values, key)
    unittest.assert.equals '' (key) (found)
end

function tests:test_strcmp ()
    local values, key = {"some","example","strings","here"}, "example"
    libc.stdlib.qsort(values, libc.string.strcmp)
    local found = libc.stdlib.bsearch(values, key, libc.string.strcmp)
    unittest.assert.equals '' (key) (found)
end

function tests:test_qsort ()
	
	local tbl = {}
	local n = 1000

	for i=1,n do table.insert(tbl, math.random(n)) end	-- prepare the input.

	local sorted, perm = libc.stdlib.qsort(tbl)

	local permuted = {}
	for i, p in ipairs(perm) do permuted[i] = tbl[p] end

	table.sort(tbl)

    for i = 1, n do
        unittest.assert.equals '' (tbl[i]) (sorted[i])
        unittest.assert.equals '' (tbl[i]) (permuted[i])
    end
end

function tests:test_10_20_30 ()
	
	local x, y, z = 10.0, 20.0, 30.0
	unittest.assert.equals '' (x * y + z) (libc.math.fma(x, y, z))
end

function tests:test_a_b_minus ()
	
	local x, y = 1.0, 0.000001
    local high = x * y
    local low = libc.math.fma(x, y, -high)
	unittest.assert.equals '' (x * y) (high + low)
    unittest.assert.equals '' (x * y) (libc.math.fma (x, y, 0.0))
end

function tests:test_math_constants ()
	unittest.assert.equals '' (3.14159265358979323846) (libc.math.M_PI)
	unittest.assert.equals '' (1.618033988749894848204586834365638117720309179805762) (libc.math.M_GR)
end

function tests:test_pthread_create ()

    local witness = {}

    local pth = libc.pthread.create {} (function () return witness end)

    unittest.assert.equals 'expect a thread and a userdata' 
        ('thread', 'userdata') (type (pth.thread), type (pth.pthread))
    
    unittest.assert.equals 'expect the witness to be returned' 
        (true, witness) (libc.pthread.join (pth))

    unittest.assert.equals 'expect the thread to be joined' 
        (nil, libc.stddef.NULL) (pth.thread, pth.pthread)
end

function tests:test_pthread_create_error ()

    local witness = 'a simple induced error'

    local pth = libc.pthread.create {} (function () error (witness, 0) end)

    unittest.assert.equals '' ('thread', 'userdata') (type (pth.thread), type (pth.pthread))
    unittest.assert.equals '' (false, witness) (libc.pthread.join (pth))
    unittest.assert.equals '' (nil, libc.stddef.NULL) (pth.thread, pth.pthread)
end

function tests:test_pthread_create_dispatched ()

    local continue = true

    local pth = libc.pthread.create { create_detached = true } (function () while continue do end end)

    unittest.assert.equals '' ('thread', 'userdata' ) (type (pth.thread), type (pth.pthread))

    continue = false

    unittest.assert.equals '' (true) (libc.pthread.join (pth))
end

print (unittest.api.suite (tests))