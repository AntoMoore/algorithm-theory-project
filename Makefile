EXECS = args sha512

sha512: sha512.c
	cc -o sha512 sha512.c

test: test.sh
	./test.sh

clean:
	rm -f $(EXECS)