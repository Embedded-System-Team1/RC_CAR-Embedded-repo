gcc -o rc_server ./server/rc_server.c -lwebsockets -lrt
gcc -o controller controller.c -lpthread -lwiringPi -ljansson
gcc -o handler handler.c -lpthread -lwiringPi -ljansson
sudo ./main