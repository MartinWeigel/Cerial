CC = gcc
CFLAGS += -std=c11 -Wall -Wextra -pedantic -O3
CLIBS = -lm
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

ifeq ($(OS), Windows_NT)
BIN = example.exe
RM = del /F
else
BIN = example
RM = rm -f
endif

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(CLIBS)

.PHONY: clean
clean:
	$(RM) $(BIN) $(OBJS)
