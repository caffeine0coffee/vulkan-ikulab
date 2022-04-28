Cflags = -std=c++17 -fdiagnostics-color=always -g
LdFlags = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

SrcPaths := $(wildcard src/*.cpp) $(wildcard src/shape/*.cpp)
Ingredients := $(SrcPaths) $(wildcard src/*.hpp) $(wildcard src/shape/*.hpp) $(wildcard shaders/bin/*.spv)
IncludePath := ./includes


bin/app: $(Ingredients)
	g++ $(Cflags) -O0 -o bin/app $(SrcPaths) $(LdFlags) -I$(IncludePath)

shaders/bin/vert.spv: shaders/shader.vert
	/usr/local/bin/glslc shader.vert -o shaders/bin/vert.spv

shaders/bin/frag.spv: shaders/shader.frag

	/usr/local/bin/glslc shader.frag -o shaders/bin/frag.spv


.PHONY: clean release run

release: $(Ingredients)
	./compile.sh
	g++ $(Cflags) -O3 -o bin/app $(SrcPaths) $(LdFlags) -I$(IncludePath)

clean:
	rm bin/app
	rm shaders/bin/*

run: bin/app
	bin/app
