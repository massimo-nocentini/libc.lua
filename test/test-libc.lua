
local lu = require 'luaunit'
local libc = require 'libc'
local lambda = require 'operator'

Test_base64 = {}

function Test_base64:test_l64a ()
	lu.assertEquals (libc.stdlib.l64a(32), 'U')
	lu.assertEquals (libc.stdlib.l64a(23948792), 'srKP/')
	lu.assertEquals (libc.stdlib.l64a(0), '')
end

function Test_base64:test_a64l ()
    lu.assertEquals (libc.stdlib.a64l('U'), 32)
    lu.assertEquals (libc.stdlib.a64l('srKP/'), 23948792)
end

function Test_base64:test_a64l_l64a_identity ()
    lu.assertEquals (libc.stdlib.a64l(libc.stdlib.l64a(32)), 32)
    lu.assertEquals (libc.stdlib.a64l(libc.stdlib.l64a(23948792)), 23948792)
end

--------------------------------------------------------------------------------

Test_lldiv = {}

function Test_lldiv:test_minus5_3 ()
    local q, r = libc.stdlib.lldiv(-5, 3)
    lu.assertEquals (q, -1)
    lu.assertEquals (r, -2)
end

function Test_lldiv:test_4897294869528760942_759843276952576 ()
    local m, n = 4897294869528760942, 759843276952576
    local q, r = libc.stdlib.lldiv(m, n)
    lu.assertEquals (q, 6445)
    lu.assertEquals (r, 104949569408622)
    lu.assertEquals (q * n + r, m)
end

--------------------------------------------------------------------------------

Test_bsearch = {}

function Test_bsearch:test_lteqgtcmp ()
    local values, key = { 50, 20, 60, 40, 10, 30 }, 40
    libc.stdlib.qsort(values)
    local found = libc.stdlib.bsearch(values, key)
    lu.assertEquals (found, key)
end

function Test_bsearch:test_strcmp ()
    local values, key = {"some","example","strings","here"}, "example"
    libc.stdlib.qsort(values, libc.string.strcmp)
    local found = libc.stdlib.bsearch(values, key, libc.string.strcmp)
    lu.assertEquals (found, key)
end

--------------------------------------------------------------------------------

Test_qsort = {}

function Test_qsort:test_1e6 ()
	
	local tbl = {}
	local n = 1000000 

	for i=1,n do table.insert(tbl, math.random(n)) end	-- prepare the input.

	local sorted, perm = libc.stdlib.qsort(tbl)

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
	lu.assertEquals(libc.math.fma(x, y, z), x * y + z)
end


function Test_fma:test_a_b_minus ()
	
	local x, y = 1.0, 0.000001
    local high = x * y
    local low = libc.math.fma(x, y, -high)
	lu.assertEquals(high + low, x * y)
    lu.assertEquals(libc.math.fma (x, y, 0.0), x * y)
end

--------------------------------------------------------------------------------

Test_constants = {}

function Test_constants:test_gr ()
	
	lu.assertEquals(libc.math.M_GR, 1.618033988749894848204586834365638117720309179805762)
end

function Test_constants:test_pi ()
	
    lu.assertEquals(libc.math.M_PI, 3.14159265358979323846)
end

--------------------------------------------------------------------------------
Test_pthread = {}

function Test_pthread:test_pthread_create ()
	
    local a, j = 0, 100

	local pthread = libc.pthread.checked_create
        'pthread_create failed.' (function () for i = 1, j do a = a + 1 end end)

    local v = libc.pthread.checked_join 'pthread_join failed.' (pthread)

    lu.assertEquals (a, j)
    lu.assertNil (v)
end

function Test_pthread:test_pthread_sleep ()

    local a = 0

    local pthread = libc.pthread.checked_create 'pthread_create failed.' (
        lambda.o { function () return a end, os.execute }, 'sleep 2'
    )

    local pthread_print = libc.pthread.checked_create 'pthread_create failed.' (
        lambda.o {
            function () while true do a = a + 1 end end,
            libc.pthread.assert 'pthread_selfdetach failed.',
            libc.pthread.selfdetach,
        } 
    )
    
    local v = libc.pthread.checked_join 'pthread_join failed.' (pthread)

    lu.assertTrue (v <= a)
end

function Test_pthread:test_pthread_create_named_function ()

    local a, j, s = 0, 100, 'hello from main'

    local function A (k, b)
        for i = 1, k do a = a + 1 end
        return a, #b
    end

	local pthread = libc.pthread.checked_create 'pthread_create failed.' (A, j, s)

    local ra, rs, useless = libc.pthread.checked_join 'pthread_join failed.' (pthread)

    lu.assertEquals (a, j)
    lu.assertEquals (a, ra)
    lu.assertEquals (#s, rs)
    lu.assertNil (useless)
end


function Test_pthread:test_pthread_self ()

	local pthread = libc.pthread.self ()

    lu.assertEquals (type (pthread.pthread), 'userdata')
end

function Test_pthread:test_pthread_equal_self ()
	
    lu.assertTrue (libc.pthread.equal (libc.pthread.self (), libc.pthread.self ()))
end

function Test_pthread:test_pthread_equal ()

    local function A (main_thread)
        return libc.pthread.equal (libc.pthread.self (), main_thread)
    end

	local pthread = libc.pthread.checked_create 'pthread_create failed.' (A, libc.pthread.self ())
    lambda.o { lu.assertFalse, libc.pthread.checked_join 'pthread_join failed.' } (pthread)

end

function Test_pthread:test_pthread_coro ()

    local one, two = 1, 2

    local coroA =  coroutine.create (function (i) 
        while true do 
            print ('A ready ' .. i)
            i = coroutine.yield (one)
            --os.execute ('sleep ' .. random.random(i))
        end 
    end)
    
    local coroB =  coroutine.create (function (i) 
        while true do 
            print ('B ready ' .. i); 
            i = coroutine.yield (two);
            --os.execute ('sleep ' .. random.random(10))
        end 
    end)
    
    local f = function (coro, j)
        for i = 1, j do
            local flag, v = coroutine.resume (coro, i)
            if flag and v ~= nil then print ('C', v) end
        end
    end

	local pthread_one = libc.pthread.checked_create 'pthread_create failed.' (f, coroA, 10)
    local pthread_two = libc.pthread.checked_create 'pthread_create failed.' (f, coroB, 10)
    
    libc.pthread.checked_join 'pthread_join failed.' (pthread_one)
    libc.pthread.checked_join 'pthread_join failed.' (pthread_two)

end


--------------------------------------------------------------------------------

os.exit( lu.LuaUnit.run() )
