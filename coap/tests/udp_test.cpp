#include <sys/socket.h>
#include <cstdlib>
#include "udp_server.hpp"

int main(int argc, char ** argv)
{
    udp_server server ("192.168.0.100", 5673);

    server.run();

    return EXIT_SUCCESS;
}
