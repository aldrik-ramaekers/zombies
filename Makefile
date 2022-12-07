main:
	rm -rf "build/"
	mkdir -p "build/"
	gcc -m64 -g -DMODE_DEBUG main.c -o build/zombies.exe -lprojectbase-debug  -Llibs/ -lSDL2 -lSDL2_mixer
	./build/zombies.exe
