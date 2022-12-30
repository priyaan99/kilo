CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
OBJS = main.o
NAME = kilo

all: kilo

kilo: $(OBJS)
	$(CC) -o $(NAME) $(OBJS) 

main.o: main.c 
	$(CC) -c $< $(CFLAGS)
 
clean:
	rm $(OBJS) $(NAME)
