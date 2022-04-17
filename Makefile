CFLAGS = -std=c++17 -O2 -fdiagnostics-color=always -g
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
INCLUDE_PATH = ./includes

app: main.cpp FirstTriangleApp.cpp FirstTriangleApp.hpp shaders/shader.*
	./compile.sh
	g++ $(CFLAGS) -o app main.cpp FirstTriangleApp.cpp $(LDFLAGS) -I$(INCLUDE_PATH)

run: app
	./app

sample: sample.cpp
	./compile.sh
	g++ $(CFLAGS) -o sample sample.cpp $(LDFLAGS) -I$(INCLUDE_PATH)

run_sample: sample
	./sample

.PHONY: clean

clean:
	rm -f app sample
	rm shaders/bin/*