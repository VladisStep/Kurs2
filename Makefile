all:
	gcc myProg.c -lpng && ./a.out --draw_circle -S 500 500 -R 100 -W 100 -C red newFile.png