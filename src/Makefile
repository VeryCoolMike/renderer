CC = clang
CFLAGS = -I./glad/include
LDFLAGS = -lglfw -lm
SRC = main.cpp glad/src/glad.c
OUT = main

$(OUT): $(SRC)
	$(CC) $(SRC) $(CFLAGS) -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT)
