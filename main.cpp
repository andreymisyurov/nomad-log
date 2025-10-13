#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include "server.h"

int main() {
    Server server(8080);
    server.start();
    server.run();
    return 0;
}
