all:
	gcc myProg.c -lpng && ./a.out --reflect -S 50 -E 600 500 -O y newFile.png