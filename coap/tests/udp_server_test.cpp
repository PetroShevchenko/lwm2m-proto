#include <sys/socket.h>
#include <cstdlib>
#include "udp_server.hpp"

int main(int argc, char ** argv)
{
	try {
    udp_server server ("127.0.0.1", 5673);

    server.run();
    }
    catch(...)
    {
    	std::cerr << "SERVER: was cought the error from exception";
    }
    return EXIT_SUCCESS;
}
