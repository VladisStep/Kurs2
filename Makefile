all:
	gcc myProg.c -lpng && ./a.out spongebob.png -s -R 1024 newFile.png
