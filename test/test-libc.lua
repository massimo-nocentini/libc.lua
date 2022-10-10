
local libc = require 'libc'

assert (libc.l64a(32), 'U')
assert (libc.l64a(329547523879582), 'S8D411')

assert (libc.a64l(libc.l64a(32)), 32)
assert (libc.a64l(libc.l64a(329547523879582)), 329547523879582)
