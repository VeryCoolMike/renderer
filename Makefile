CC = clang
CFLAGS = -I./include -I./glad/include -I./imgui -I./imgui/backends
LDFLAGS = -lglfw -lm -lstdc++ -llua
SRC = main.cpp glad/src/glad.c imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/backends/imgui_impl_glfw.cpp imgui/backends/imgui_impl_opengl3.cpp
OBJ = main.o glad.o imgui.o imgui_draw.o imgui_tables.o imgui_widgets.o imgui_impl_glfw.o imgui_impl_opengl3.o
OUT = main

$(OUT): $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o $(OUT) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $< $(CFLAGS)

glad.o: glad/src/glad.c
	$(CC) -c $< $(CFLAGS)

imgui.o: imgui/imgui.cpp
	$(CC) -c $< $(CFLAGS)

imgui_draw.o: imgui/imgui_draw.cpp
	$(CC) -c $< $(CFLAGS)

imgui_tables.o: imgui/imgui_tables.cpp
	$(CC) -c $< $(CFLAGS)

imgui_widgets.o: imgui/imgui_widgets.cpp
	$(CC) -c $< $(CFLAGS)

imgui_impl_glfw.o: imgui/backends/imgui_impl_glfw.cpp
	$(CC) -c $< $(CFLAGS)

imgui_impl_opengl3.o: imgui/backends/imgui_impl_opengl3.cpp
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f $(OUT)


