CC = clang
CFLAGS = -I./include -I./glad/include -I./include/imgui -I./include/imgui/backends
LDFLAGS = -lglfw -lm -lstdc++ -llua
SRC = main.cpp glad/src/glad.c include/imgui/imgui.cpp include/imgui/imgui_draw.cpp include/imgui/imgui_tables.cpp include/imgui/imgui_widgets.cpp include/imgui/backends/imgui_impl_glfw.cpp include/imgui/backends/imgui_impl_opengl3.cpp
OBJ = main.o glad.o imgui.o imgui_draw.o imgui_tables.o imgui_widgets.o imgui_impl_glfw.o imgui_impl_opengl3.o
OUT = main

$(OUT): $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o $(OUT) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $< $(CFLAGS)

glad.o: glad/src/glad.c
	$(CC) -c $< $(CFLAGS)

imgui.o: include/imgui/imgui.cpp
	$(CC) -c $< $(CFLAGS)

imgui_draw.o: include/imgui/imgui_draw.cpp
	$(CC) -c $< $(CFLAGS)

imgui_tables.o: include/imgui/imgui_tables.cpp
	$(CC) -c $< $(CFLAGS)

imgui_widgets.o: include/imgui/imgui_widgets.cpp
	$(CC) -c $< $(CFLAGS)

imgui_impl_glfw.o: include/imgui/backends/imgui_impl_glfw.cpp
	$(CC) -c $< $(CFLAGS)

imgui_impl_opengl3.o: include/imgui/backends/imgui_impl_opengl3.cpp
	$(CC) -c $< $(CFLAGS)


clean:
	rm -f $(OUT)


