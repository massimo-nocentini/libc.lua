
local liblibc = require 'liblibc'
local lambda = require 'operator'

local libc = {}

libc.stddef = liblibc.stddef

libc.stdlib = {
	l64a = liblibc.l64a,
	a64l = liblibc.a64l,
	lldiv = liblibc.lldiv,
}

libc.string = {
	strcmp = liblibc.strcmp,
	strtok_r = function (str, delims, insert_empty) 
		
		delims = delims or ''
		
		if insert_empty == nil then insert_empty = false end -- because this is the normal behavior of the C function.

		return liblibc.strtok_r (str, delims, insert_empty) 
	end,
}

function string.lines (str, include)
	if include == nil then include = false end
	return libc.string.strtok_r (str, '\n', include)
end

string.tokenize = libc.string.strtok_r

libc.pthread = {
	create = function (attr_tbl) return function (f) return liblibc.pthread_create (attr_tbl, f) end end,
	join = liblibc.pthread_join,
	self = liblibc.pthread_self,
	equal = liblibc.pthread_equal,
	detach = liblibc.pthread_detach,
	cancel = liblibc.pthread_cancel,
	mutex_init = liblibc.pthread_mutex_init,
	mutex_lock = liblibc.pthread_mutex_lock,
	mutex_unlock = liblibc.pthread_mutex_unlock,
	cond_signal = liblibc.pthread_cond_signal,
	cond_broadcast = liblibc.pthread_cond_broadcast,
	cond_wait = liblibc.pthread_cond_wait,
}

setmetatable (libc.pthread, {__index = liblibc.pthread})

function libc.pthread.assert (msg)
	return function (retcode, ...)
		if retcode == 0 then return ...
		elseif retcode > 0 then error (msg)
		else error 'Not expected to reach this branch using pthreads.' end
	end
end

function libc.pthread.checked_create (msg)
	return lambda.o { 
		libc.pthread.assert (msg),
		libc.pthread.create,
	}
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

function libc.pthread.checked_mutex_lock (msg)
	return lambda.o {
		libc.pthread.assert (msg),
		libc.pthread.mutex_lock,
	}
end

function libc.pthread.checked_mutex_unlock (msg)
	return lambda.o {
		libc.pthread.assert (msg),
		libc.pthread.mutex_unlock,
	}
end

function libc.pthread.checked_mutex_trylock (msg)
	return lambda.o {
		libc.pthread.assert (msg),
		libc.pthread.mutex_trylock,
	}
end

function libc.pthread.checked_cond_signal (msg)
	return lambda.o {
		libc.pthread.assert (msg),
		libc.pthread.cond_signal,
	}
end

function libc.pthread.checked_cond_broadcast (msg)
	return lambda.o {
		libc.pthread.assert (msg),
		libc.pthread.cond_broadcast,
	}
end

function libc.pthread.checked_cond_wait (msg)
	return lambda.o {
		libc.pthread.assert (msg),
		libc.pthread.cond_wait,
	}
end

function libc.pthread.checked_cond_init (msg)
	return lambda.o {
		libc.pthread.assert (msg),
		liblibc.pthread_cond_init,
	}
end

function libc.pthread.checked_cond_destroy (msg)
	return lambda.o {
		libc.pthread.assert (msg),
		liblibc.pthread_cond_destroy,
	}
end

function libc.pthread.checked_attr_init (msg)
	return lambda.o {
		libc.pthread.assert (msg),
		liblibc.pthread_attr_init,
	}
end

function libc.pthread.checked_attr_destroy (msg)
	return lambda.o {
		libc.pthread.assert (msg),
		liblibc.pthread_attr_destroy,
	}
end

function libc.pthread.checked_mutexattr_init (msg)
	return lambda.o {
		libc.pthread.assert (msg),
		liblibc.pthread_mutexattr_init,
	}
end

function libc.pthread.checked_mutexattr_destroy (msg)
	return lambda.o {
		libc.pthread.assert (msg),
		liblibc.pthread_mutexattr_destroy,
	}
end

function libc.pthread.checked_mutex_init (msg)
	return lambda.o {
		libc.pthread.assert (msg),
		liblibc.pthread_mutex_init,
	}
end

function libc.pthread.checked_mutex_destroy (msg)
	return lambda.o {
		libc.pthread.assert (msg),
		liblibc.pthread_mutex_destroy,
	}
end

function libc.pthread.with_cond (f, h)

	h = h or error

	return function (...)

		local cond = libc.pthread.checked_cond_init 'pthread_cond_init failed.' ()
		
		local function D () 
			return libc.pthread.checked_cond_destroy 'pthread_cond_destroy failed.' (cond) 
		end

		return lambda.o {
			lambda.precv_before (D, h),
			pcall,
			lambda.ellipses_append (f),
			lambda.ellipses_append (cond),
		}
		(...)
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

setmetatable (libc.math, {__index = liblibc.math})

return libc
