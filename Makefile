main:
	rm -rf "build/"
	mkdir -p "build/"
	cp -a "data/." "build/data"
	gcc -m64 -g -Wl,--stack,4194304 -DMODE_DEBUG main.c -o build/zombies.exe -lSDL2_mixer -lSDL2
	cp -a "build/." "C:/Manually installed programs/zombieshooter" 
	./build/zombies.exe -ip 127.0.0.1 -port 27015

menu:
	rm -rf "build/"
	mkdir -p "build/"
	cp -a "data/." "build/data"
	gcc -m64 -g -Wl,--stack,4194304 -DMODE_DEBUG main.c -o build/zombies.exe -lSDL2_mixer -lSDL2
	cp -a "build/." "C:/Manually installed programs/zombieshooter" 
	./build/zombies.exe

debug_client:
	./build/zombies.exe -ip 127.0.0.1 -port 27015 &
	gdb --args ./build/zombies.exe -ip 172.29.64.1 -port 27015