main:
	rm -rf "build/"
	mkdir -p "build/"
	cp -a "data/." "build/data"
	gcc -m64 -g -DMODE_DEBUG main.c -o build/zombies.exe -lprojectbase-debug
	./build/zombies.exe -ip 127.0.0.1 -port 27015

