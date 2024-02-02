
local unittest = require 'unittest'
local libc = require 'libc'
local lambda = require 'operator'

local tests = {}

function tests:test_l64a ()
	unittest.assert.equals '' 'U' (libc.stdlib.l64a(32))
	unittest.assert.equals '' 'srKP/' (libc.stdlib.l64a(23948792))
	unittest.assert.equals '' '' (libc.stdlib.l64a(0))
end

function tests:test_a64l ()
    unittest.assert.equals '' (32) (libc.stdlib.a64l('U'))
    unittest.assert.equals '' (23948792) (libc.stdlib.a64l('srKP/'))
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

function tests:test_1e6 ()
	
	local tbl = {}
	local n = 1000000 

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

function tests:test_gr ()
	
	unittest.assert.equals '' (1.618033988749894848204586834365638117720309179805762) (libc.math.M_GR)
end

function tests:test_pi ()
	
    unittest.assert.equals '' (3.14159265358979323846) (libc.math.M_PI)
end

--------------------------------------------------------------------------------
Test_pthread = {}

function Test_pthread:test_pthread_create ()

    local a, j = 0, 100

	local pthread = libc.pthread.checked_create
        'pthread_create failed.' 
        (nil, function () for i = 1, j do a = a + 1 end end)

    local v = libc.pthread.checked_join 'pthread_join failed.' (pthread)

    unittest.assert.equals '' (a, j)
    unittest.assertNil (v)
end

function Test_pthread:test_pthread_sleep ()

    local a, continue = 0, true
    local function inc () a = a + 1 end
    local function get_a () return a end

    local pthread = libc.pthread.checked_create 'pthread_create failed.' 
        (nil, lambda.o { get_a , os.execute }, 'sleep 1')

    local pthread_print = libc.pthread.checked_create 'pthread_create failed.' 
        (nil, lambda.o {
            function () while continue do inc() end return a end,
            --libc.pthread.assert 'pthread_self failed.',
            --libc.pthread.self,
            --lambda.K (libc.pthread.detach),
        })
        

    libc.pthread.checked_detach 'Unable to detach the worker thread.' (pthread_print)
    
    --unittest.assertFalse (libc.pthread.attribute (pthread).detachstate)

    local v = libc.pthread.checked_join 'pthread_join failed.' (pthread)

    --unittest.assertFalse (libc.pthread.attribute (pthread).detachstate)

    unittest.assertTrue (v <= a)

    --unittest.assertFalse (libc.pthread.attribute (pthread_print).detachstate)

    local retcode = libc.pthread.join (pthread_print)

    --unittest.assertFalse (libc.pthread.attribute (pthread_print).detachstate)

    unittest.assert.equals '' (retcode, 22)

    --libc.pthread.checked_cancel 'Unable to cancel the worker thread.' (pthread_print)

    continue = false    -- this is necessary to stop the worker thread, otherwise a segmentation fault occurs.
end


function Test_pthread:test_pthread_sleep_attr ()

    local a, continue = 0, true
    local function inc () a = a + 1 end
    local function get_a () return a end

    local pthread = libc.pthread.checked_create 'pthread_create failed.' 
        (nil, lambda.o { get_a , os.execute }, 'sleep 1')

    local attr = libc.pthread.checked_attr_init 'pthread_attr_init failed.' 
                    { setdetachstate = libc.pthread.create_detached, }
    local pthread_print = libc.pthread.checked_create 'pthread_create failed.'
                            (attr, function () while continue do inc() end return a end)
    libc.pthread.checked_attr_destroy 'pthread_attr_destroy failed.' (attr)
    
    local v = libc.pthread.checked_join 'pthread_join failed.' (pthread)

    unittest.assertTrue (v <= a)

    local retcode = libc.pthread.join (pthread_print)

    unittest.assert.equals '' (retcode, 22)

    continue = false    -- this is necessary to stop the worker thread, otherwise a segmentation fault occurs.
end

function Test_pthread:test_pthread_create_named_function ()

    local a, j, s = 0, 100, 'hello from main'

    local function A (k, b)
        for i = 1, k do a = a + 1 end
        return a, #b
    end

	local pthread = libc.pthread.checked_create 'pthread_create failed.' (nil, A, j, s)

    local ra, rs, useless = libc.pthread.checked_join 'pthread_join failed.' (pthread)

    unittest.assert.equals '' (a, j)
    unittest.assert.equals '' (a, ra)
    unittest.assert.equals '' (#s, rs)
    unittest.assertNil (useless)
end

function Test_pthread:test_pthread_self ()

    local one, two = 1, 2

	local a, b, c = libc.pthread.self (
        function (pthread)
            local pt, ud = pthread ()
            unittest.assert.equals '' (type (pt), 'userdata')
            return one, two     -- this is just to check of LUA_MULTRET works.
        end
    )

    unittest.assert.equals ''(a, one)
    unittest.assert.equals ''(b, two)
    unittest.assertNil (c)
end

function Test_pthread:test_pthread_equal ()

    local function A (main_thread)
        return libc.pthread.self (
            function (myself) return libc.pthread.equal (myself, main_thread) end
        ) 
    end

    libc.pthread.self (function (main_thread)
        local pthread = libc.pthread.checked_create 'pthread_create failed.' (nil, A, main_thread)
        lambda.o { unittest.assertFalse, libc.pthread.checked_join 'pthread_join failed.' } (pthread)
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

	local pthread_one = libc.pthread.checked_create 'pthread_create failed.' (nil, f, coroA, 10)
    local pthread_two = libc.pthread.checked_create 'pthread_create failed.' (nil, f, coroB, 10)
    
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
        'Failed to create the first worker.' (nil, doer, 'A', N)

    local pthread_b = libc.pthread.checked_create
        'Failed to create the second worker.' (nil, doer, 'B', N)

    local v = libc.pthread.checked_join 'Failed in joining the first worker.' (pthread_a)
    local w = libc.pthread.checked_join 'Failed in joining the second worker.' (pthread_b)

    unittest.assertTrue (tot <= N * 2)
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
                
                P (mtx) -- prolaag, short for "probeer te verlagen", literally "try to reduce".
                
                local v = tot
                v = v + 1
                tot = v
            
                V (mtx) -- vrijgave, literally "release".

            end

            return times
        end

        local pthread_a = libc.pthread.checked_create 
                            'Failed creating the first worker.' 
                            (nil, doer, 'A', N)

        local pthread_b = libc.pthread.checked_create 
                            'Failed creating the second worker.' 
                            (nil, doer, 'B', N)

        local pthread_c = libc.pthread.checked_create
                            'Failed creating the third worker.'
                            (nil, doer, 'C', N)

        local v = libc.pthread.checked_join 'Failed to join the first pthread.'  (pthread_a)
        local w = libc.pthread.checked_join 'Failed to join the second pthread.' (pthread_b)
        local z = libc.pthread.checked_join 'Failed to join the third pthread.'  (pthread_c)

        assert (v == w and w == z and z == N)

        return tot
    end

    local D = lambda.without_gc (
        function ()
            local mtx = libc.pthread.checked_mutex_init 'pthread_mutex_init failed.' ()
            local _, t = pcall (T, mtx)
            libc.pthread.checked_mutex_destroy 'pthread_mutex_destroy failed.' (mtx)
            return t
        end
    )
    
    local t = D ()

    unittest.assert.equals '' (t, 3 * N)
    
end

function Test_pthread:test_pthread_mutex_cond ()

    local totThreads, numLive, numUnjoined, threads = 5, 0, 0, {}

    local P = libc.pthread.checked_mutex_lock   'pthread_mutex_lock failed.'
    local V = libc.pthread.checked_mutex_unlock 'pthread_mutex_unlock failed.'
    local S = libc.pthread.checked_cond_signal  'pthread_cond_signal failed.'
    local W = libc.pthread.checked_cond_wait    'pthread_cond_wait failed.'

    local function T (mtx)

        local function D (threadDied)

            local function A (idx)
            
                os.execute ('sleep ' .. threads [idx].sleeptime)

                print (string.format ('pthread %d terminated.', idx))

                P (mtx) 
                
                threads [idx].state = 'terminated'
                numUnjoined = numUnjoined + 1
                
                V (mtx)
                
                S (threadDied)

            end

            for i = 1, totThreads do

                local t = libc.pthread.checked_create ('Failed creating worker ' .. i)
                
                threads [i] = { sleeptime = 1, state = 'alive', idx = i, }
                threads [i].pthread = t (nil, A, i)

                numLive = numLive + 1

            end

            while numLive > 0 do
                
                P (mtx)

                while numUnjoined == 0 do W (threadDied, mtx) end

                for i = 1, totThreads do

                    local t = threads [i]

                    if t.state == 'terminated' then

                        libc.pthread.checked_join ('Failed to join thread ' .. t.idx) (t.pthread)
                        
                        t.state = 'joined'
                        
                        numLive = numLive - 1
                        numUnjoined = numUnjoined - 1

                        print (string.format ("Reaped thread %d (numLive=%d)\n", t.idx, numLive))
                    end
    
                end

                V (mtx)
            end
            
        end

        return libc.pthread.with_cond (D, error) ()

    end

    local D = lambda.without_gc (
        function ()
            local mtx = libc.pthread.checked_mutex_init 'pthread_mutex_init failed.' ()
            local _, t = pcall (T, mtx)
            libc.pthread.checked_mutex_destroy 'pthread_mutex_destroy failed.' (mtx)
            return t
        end
    )
    
    local t = D ()
    
end

--------------------------------------------------------------------------------

print (unittest.api.suite (tests))