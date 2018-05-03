frogger : main.o frogger.o console.o linkedlist.o player.o log.o gameglobals.o threadwrappers.o
	clang -Wall -g -lcurses -pthread -o frogger main.o frogger.o console.o player.o log.o linkedlist.o gameglobals.o threadwrappers.o

main.o : main.c
	clang -Wall -g -pthread -c main.c

frogger.o : frogger.c 
	clang -Wall -g -pthread -c frogger.c

console.o : console.c
	clang -Wall -g -pthread -c console.c

linkedlist.o : linkedlist.c
	clang -Wall -g -pthread -c linkedlist.c

player.o : player.c
	clang -Wall -g -pthread -c player.c

log.o : log.c
	clang -Wall -g -pthread -c log.c linkedlist.c

gameglobals.o : gameglobals.c
	clang -Wall -g -pthread -c gameglobals.c

threadwrappers.o : threadwrappers.c
	clang -Wall -g -pthread -c threadwrappers.c

clean :
	rm *.o frogger
