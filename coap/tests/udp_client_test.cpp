#include <sys/socket.h>
#include <cstdlib>
#include <iostream>
#include "udp_client.hpp"

int main(int argc, char ** argv)
{


	try {
    udp_client client ("0.0.0.0", 5673);

    client.send((const uint8_t *)"HELLO SERVER", sizeof("HELLO SERVER") - 1);
    
    client.receive();
    
    std::cout << (const char *)client.get_buffer() << std::endl;

    client.send((const uint8_t *)"ACK", sizeof("ACK") - 1);

	}
	catch(...) {
		std::cerr << "CLIENT: was cought the error from exception";
	}

    return EXIT_SUCCESS;
}