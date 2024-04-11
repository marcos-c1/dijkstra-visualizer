flags=-lGL -lglut -lGLU -lm -std=c99
program=main.c
djk=lib/dijkstra.c
settings=lib/settings.c

run: link main 

link:
	gcc -c $(settings) -o settings.o $(flags)
	gcc -c $(djk) -o dijkstra.o $(flags)

main:
	gcc $(program) -o main.o dijkstra.o settings.o $(flags) && ./main.o

rm:
	rm *.o
