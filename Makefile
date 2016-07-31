all: clean compile run

clean:
	rm -f ./build/*

compile:
	mkdir -p build
	g++ *.cpp -o ./build/raytracer.out

run:
	chmod +x ./build/raytracer.out
	./build/raytracer.out
