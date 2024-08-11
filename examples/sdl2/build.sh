mkdir -p ../../dist
g++ -o ../../dist/goblin3d_example -I../../src ../../src/goblin3d.cpp sdl2_port.c -lm -lSDL2
