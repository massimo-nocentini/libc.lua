
linux:
	gcc -O2 -std=gnu11 -fPIC -Wall -s -shared -o liblibc.so liblibc.c -llua -lpthread

macos:
	cc -O2 -std=c11 -fPIC -Wall -dynamiclib -o liblibc.dylib liblibc.c -llua -lpthread

install:
	mkdir -p /usr/local/lib/lua/5.4/	# just for ensuring that the dest dir exists
	mkdir -p /usr/local/share/lua/5.4/	# just for ensuring that the dest dir exists
	mv liblibc.so /usr/local/lib/lua/5.4/
	cp libc.lua /usr/local/share/lua/5.4/

install-macos:
	mkdir -p /usr/local/lib/lua/5.4/	# just for ensuring that the dest dir exists
	mkdir -p /usr/local/share/lua/5.4/	# just for ensuring that the dest dir exists
	mv liblibc.dylib /usr/local/lib/lua/5.4/
	cp libc.lua /usr/local/share/lua/5.4/

test:
	cd ../test && lua test-libc.lua -v #> README
