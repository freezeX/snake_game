default target: screen.c game.c snake.c screen.h game.h
	gcc -std=c11 -Wall -Werror -pedantic screen.c game.c snake.c -lncurses -o snake
snake: screen.c game.c snake.c screen.h game.h
	gcc -std=c11 -Wall -Werror -pedantic screen.c game.c snake.c -lncurses -o snake
clean:
	rm -f snake
