all: toy_example profile
toy_example: toy_example.c ../cacheutils.h
	gcc -std=gnu11 -O2 -o $@ $@.c
profile: profile.c ../cacheutils.h
	gcc -std=gnu11 -O2 -o $@ $@.c
clean:
	rm toy_example profile
