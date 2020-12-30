nutsh: *.c *.h
	gcc *.c -o nutsh -std=c99 -g

install: nutsh
	cp nutsh /usr/bin/nutsh
