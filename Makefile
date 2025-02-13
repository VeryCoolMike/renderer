CC = clang
CFLAGS = -I./include -I./glad/include -I./imgui -I./imgui/backends
LDFLAGS = -lglfw -lm -lstdc++ -llua
SRC = main.cpp glad/src/glad.c imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/backends/imgui_impl_glfw.cpp imgui/backends/imgui_impl_opengl3.cpp
OUT = main

$(OUT): $(SRC)
	$(CC) $(SRC) $(CFLAGS) -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT)
