main:
	rm -rf "build/"
	mkdir -p "build/"
	gcc -m64 -g -DMODE_DEBUG main.c -o build/zombies.exe -lprojectbase-debug  -Llibs/ -lSDL2 -lSDL2_mixer -lWs2_32
	./build/zombies.exe -ip 127.0.0.1 -port 27015
	# ./build/zombies.exe -ip 172.24.224.1 -port 27015 

