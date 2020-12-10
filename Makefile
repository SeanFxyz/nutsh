nutsh: *.c *.h
	gcc -g -o nutsh *.c

install: nutsh
	cp nutsh /usr/bin/nutsh
