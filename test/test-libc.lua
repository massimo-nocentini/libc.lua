
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
        'pthread_create failed.' { start_function = function () for i = 1, j do a = a + 1 end end } ()

    local v = libc.pthread.checked_join 'pthread_join failed.' (pthread)

    lu.assertEquals (a, j)
    lu.assertNil (v)
end

function Test_pthread:test_pthread_sleep ()

    local a, continue = 0, true
    local function inc () a = a + 1 end
    local function get_a () return a end

    local pthread = libc.pthread.checked_create 'pthread_create failed.' 
        { start_function = lambda.o { get_a , os.execute }} 
        'sleep 1'

    local pthread_print = libc.pthread.checked_create 'pthread_create failed.' 
        { start_function = lambda.o {
                function () while continue do inc() end return a end,
                --libc.pthread.assert 'pthread_self failed.',
                --libc.pthread.self,
                --lambda.K (libc.pthread.detach),
            }
        }
        ()
        

    libc.pthread.checked_detach 'Unable to detach the worker thread.' (pthread_print)
    
    --lu.assertFalse (libc.pthread.attribute (pthread).detachstate)

    local v = libc.pthread.checked_join 'pthread_join failed.' (pthread)

    --lu.assertFalse (libc.pthread.attribute (pthread).detachstate)

    lu.assertTrue (v <= a)

    --lu.assertFalse (libc.pthread.attribute (pthread_print).detachstate)

    local retcode = libc.pthread.join (pthread_print)

    --lu.assertFalse (libc.pthread.attribute (pthread_print).detachstate)

    lu.assertEquals (retcode, 22)

    --libc.pthread.checked_cancel 'Unable to cancel the worker thread.' (pthread_print)

    continue = false    -- this is necessary to stop the worker thread, otherwise a segmentation fault occurs.
end


function Test_pthread:test_pthread_sleep_attr ()

    local a, continue = 0, true
    local function inc () a = a + 1 end
    local function get_a () return a end

    local pthread = libc.pthread.checked_create 'pthread_create failed.' 
        { create_joinable = true, start_function = lambda.o { get_a , os.execute } }
        'sleep 1'

    local pthread_print = libc.pthread.checked_create 'pthread_create failed.'
        { create_detached = true, start_function = function () while continue do inc() end return a end }
        ()
    
    local v = libc.pthread.checked_join 'pthread_join failed.' (pthread)

    lu.assertTrue (v <= a)

    local retcode = libc.pthread.join (pthread_print)

    lu.assertEquals (retcode, 22)

    continue = false    -- this is necessary to stop the worker thread, otherwise a segmentation fault occurs.
end

function Test_pthread:test_pthread_create_named_function ()

    local a, j, s = 0, 100, 'hello from main'

    local function A (k, b)
        for i = 1, k do a = a + 1 end
        return a, #b
    end

	local pthread = libc.pthread.checked_create 'pthread_create failed.' { start_function = A } (j, s)

    local ra, rs, useless = libc.pthread.checked_join 'pthread_join failed.' (pthread)

    lu.assertEquals (a, j)
    lu.assertEquals (a, ra)
    lu.assertEquals (#s, rs)
    lu.assertNil (useless)
end

function Test_pthread:test_pthread_self ()

    local one, two = 1, 2

	local a, b, c = libc.pthread.self (
        function (pthread)
            lu.assertEquals (type (pthread.pthread), 'userdata')
            return one, two     -- this is just to check of LUA_MULTRET works.
        end
    )

    lu.assertEquals(a, one)
    lu.assertEquals(b, two)
    lu.assertNil (c)
end

function Test_pthread:test_pthread_equal ()

    local function A (main_thread)
        return libc.pthread.self (
            function (myself) return libc.pthread.equal (myself, main_thread) end
        ) 
    end

    libc.pthread.self (function (main_thread)
        local pthread = libc.pthread.checked_create 'pthread_create failed.' { start_function = A } (main_thread)
        lambda.o { lu.assertFalse, libc.pthread.checked_join 'pthread_join failed.' } (pthread)
    end)

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
        return
    end

	local pthread_one = libc.pthread.checked_create 'pthread_create failed.' { start_function = f } (coroA, 10)
    local pthread_two = libc.pthread.checked_create 'pthread_create failed.' { start_function = f } (coroB, 10)
    
    libc.pthread.checked_join 'pthread_join failed.' (pthread_one)
    libc.pthread.checked_join 'pthread_join failed.' (pthread_two)

end

function Test_pthread:test_pthread_sync_missing ()

    local tot, N = 0, 1000000

    local function doer (actor, n)

        for j = 1, n do
            local v = tot
            v = v + 1
            tot = v
        end

        return tot
    end

	local pthread_a = libc.pthread.checked_create
        'Failed to create the first worker.' { start_function = doer } ('A', N)

    local pthread_b = libc.pthread.checked_create
        'Failed to create the second worker.' { start_function = doer } ('B', N)

    local v = libc.pthread.checked_join 'Failed in joining the first worker.' (pthread_a)
    local w = libc.pthread.checked_join 'Failed in joining the second worker.' (pthread_b)

    lu.assertTrue (tot <= N * 2)
end

function Test_pthread:test_pthread_sync_mutex ()

    local N = 100000

    local function T (mtx)

        local tot = 0   -- the guilty, "global" variable.

        local function doer (actor, n)

            local times = 0
            local P = libc.pthread.checked_mutex_lock   (actor .. ' failed to acquire the lock.')
            local V = libc.pthread.checked_mutex_unlock (actor .. ' failed to release the lock.')

            for i = 1, n do

                times = times + 1;  -- I can safely update my own local `times`.
                
                P (mtx) -- prolaag, short for probeer te verlagen, literally "try to reduce".
                do
                    local v = tot
                    v = v + 1
                    tot = v
                end
                V (mtx) -- vrijgave, literally "release".

            end

            return  times
        end

        local pthread_a = libc.pthread.checked_create 
                            'Failed creating the first worker.' 
                            { create_joinable = true, start_function = doer } 
                            ('A', N)

        local pthread_b = libc.pthread.checked_create 
                            'Failed creating the second worker.' 
                            { create_joinable = true, start_function = doer } 
                            ('B', N)

        local pthread_c = libc.pthread.checked_create
                            'Failed creating the third worker.'
                            { create_joinable = true, start_function = doer } 
                            ('C', N)

        local v = libc.pthread.checked_join 'Failed to join the first pthread.' (pthread_a)
        local w = libc.pthread.checked_join 'Failed to join the second pthread.' (pthread_b)
        local z = libc.pthread.checked_join 'Failed to join the third pthread.' (pthread_c)

        assert (v == w and w == z and z == N)

        return tot
    end

    local t = lambda.without_gc_do (function () return libc.pthread.mutex_init {} (T, error) end)

    lu.assertEquals (t, 3 * N)
    
end

--------------------------------------------------------------------------------

os.exit( lu.LuaUnit.run() )