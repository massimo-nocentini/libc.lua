
local liblibc = require 'liblibc'
local lambda = require 'operator'

local libc = {}

libc.stdlib = {
	l64a = liblibc.l64a,
	a64l = liblibc.a64l,
	lldiv = liblibc.lldiv,
}

libc.string = {
	strcmp = liblibc.strcmp,
}

libc.pthread = {
	create = liblibc.pthread_create,
	join = liblibc.pthread_join,
	self = liblibc.pthread_self,
	equal = liblibc.pthread_equal,
	detach = liblibc.pthread_detach,
	attribute = liblibc.pthread_attribute,
	cancel = liblibc.pthread_cancel,
	mutex_init = liblibc.pthread_mutex_init,
	mutex_lock = liblibc.pthread_mutex_lock,
	mutex_unlock = liblibc.pthread_mutex_unlock,
	mutex_trylock = liblibc.pthread_mutex_trylock,
}

function libc.pthread.assert (msg)
	return function (retcode, ...)
		if retcode == 0 then return ...
		elseif retcode > 0 then error (msg)
		else error 'Not expected to reach this branch using pthreads.' end
	end
end


function libc.pthread.assert1 (msg)
	return function (retcode, v)
		if retcode == 0 then return v
		elseif retcode > 0 then error (msg)
		else error 'Not expected to reach this branch using pthreads.' end
	end
end

function libc.pthread.checked_create (msg)
	return function (tbl)
		return function (f)
			return lambda.o { 
				libc.pthread.assert (msg),
				libc.pthread.create (tbl) (f),
			}
		end
	end
end

function libc.pthread.checked_join (msg)
	return lambda.o {
		libc.pthread.assert (msg),
		libc.pthread.join,
	}
end

function libc.pthread.checked_detach (msg)
	return lambda.o {
		libc.pthread.assert (msg),
		libc.pthread.detach,
	}
end

function libc.pthread.checked_cancel (msg)
	return lambda.o {
		libc.pthread.assert (msg),
		libc.pthread.cancel,
	}
end

function libc.pthread.critical_section (f)

	local pthread = libc.pthread

	return function (mtx, ...)
		--lambda.o { libc.pthread.assert 'Failed acquiring the mutex.', libc.pthread.mutex_lock } (mtx)
		--libc.pthread.assert1 'Failed acquiring the mutex.' (libc.pthread.mutex_lock (mtx))
		pthread.mutex_lock (mtx)
		local v = table.pack (f ())
		--lambda.o { libc.pthread.assert 'Failed releasing the mutex.', libc.pthread.mutex_unlock } (mtx)
		--libc.pthread.assert1 'Failed releasing the mutex.' (libc.pthread.mutex_unlock (mtx))
		pthread.mutex_unlock (mtx)
		return table.unpack (v)
	end

end

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
