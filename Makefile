CC = gcc
# CFLAGS= -I.
NAME = loader
# LIBS =  -lpthread
SRCS := $(wildcard *.c)
OBJS := $(SRC:.c=.o)

all: clean
	$(CC) $(SRCS) $(CFLAGS) $(LIBS) -o $(NAME)

clean:
	rm -f *.gch $(NAME)