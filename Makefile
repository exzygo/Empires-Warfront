CC 		= gcc
CFLAGS	= -Wall -Wextra -I./src/headers
LIBS 	= -lraylib -lm -lGL -lpthread -ldl -lrt -lX11
OUT 	= ewgame
SRC 	= $(wildcard src/*.c)

all: $(OUT)

$(OUT): $(SRC)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f $(OUT)
