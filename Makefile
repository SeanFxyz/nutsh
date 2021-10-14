nutsh: *.c *.h
	cc *.c -o nutsh -std=c99 -D _ERR_CHECK -Wall

debug: *.c *.h
	cc *.c -o nutsh -std=c99 -g -D _ERR_CHECK -Wall

install: nutsh
	cp nutsh /usr/bin/nutsh
