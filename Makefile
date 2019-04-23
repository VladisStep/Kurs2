all:
	gcc myProg.c -lpng && ./a.out --reflect -S 100 200 -E 500 500