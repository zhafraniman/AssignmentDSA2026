all:
	g++ main.cpp player.cpp renderer.cpp dialogue.cpp map.cpp -o game -I/c/raylib/raylib/include -L/c/raylib/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm -static-libgcc -static-libstdc++

