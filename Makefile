main:
	rm -rf "build/"
	mkdir -p "build/"
	cp -a "data/." "build/data"
	gcc -m64 -g -Wl,--stack,4194304 -DMODE_DEBUG main.c -o build/zombies.exe -lSDL2_mixer -lSDL2
	cp -a "build/." "C:/Manually installed programs/zombieshooter" 
	#gdb -ex run --args ./build/zombies.exe -ip 127.0.0.1 -port 27015
	./build/zombies.exe -ip 127.0.0.1 -port 27015

menu:
	rm -rf "build/"
	mkdir -p "build/"
	cp -a "data/." "build/data"
	gcc -m64 -g -Wl,--stack,4194304 -DMODE_DEBUG main.c -o build/zombies.exe -lSDL2_mixer -lSDL2
	cp -a "build/." "C:/Manually installed programs/zombieshooter" 
	gdb -ex run ./build/zombies.exe

debug_client:
	./build/zombies.exe -ip 127.0.0.1 -port 27015 &
	./build/zombies.exe -ip 172.22.64.1 -port 27015