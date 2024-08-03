NAME=viewer


all: build run


build:
	@gcc -o $NAME main.c viewer.c -lm -lSDL2 -lSDL2_image -lSDL2_ttf



clean:
	@rm ./$NAME


run:
	@./$NAME
