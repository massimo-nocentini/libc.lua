
local libc = require 'libc'
local lambda = require 'operator'

--  local a, j = 0, 100

--  local pthread = libc.pthread.checked_create
--      'pthread_create failed.' {} (function () for i = 1, j do a = a + 1 end return a, 2 end) ()

--  local v = libc.pthread.checked_join 'pthread_join failed.' (pthread)

--  print (v)

local N = 1000000

-- collectgarbage 'stop'

local function T (mtx) 

    local tot = 0

    local function doer (n)

        local times = 0

        local D = libc.pthread.critical_section (function (i) 
            local v = tot
            v = v + 1
            tot = v
        end)

        lambda.fromtodo (1, n) (function (i) times = times + 1; D(mtx, i) end)

        return  times
    end

    local pthread_a = libc.pthread.checked_create 'Failed creating the first worker.' 
                                                  ({ create_joinable = true, }, doer) (N)

    local pthread_b = libc.pthread.checked_create 'Failed creating the second worker.' 
                                                  ({ create_joinable = true, }, doer) (N)

    local pthread_c = libc.pthread.checked_create 'Failed creating the third worker.' 
                                                  ({ create_joinable = true, }, doer) (N)                                            

    local _, v = libc.pthread.join  (pthread_a)
    local _, w = libc.pthread.join  (pthread_b)
    local _, z = libc.pthread.join  (pthread_c)

    assert (v == w and w == z and z == N)

    return tot
end

local t = libc.pthread.mutex_init {} (T, error)

print (t)

-- collectgarbage 'restart'