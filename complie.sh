gcc -o controller controller.c -lpthread -lwiringPi -ljansson
gcc -o handler handler.c -lpthread -lwiringPi -ljansson -lwebsockets
sudo ./main