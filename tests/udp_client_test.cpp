#include <sys/socket.h>
#include <cstdlib>
#include <iostream>
#include "udp_client.hpp"
#include "udp_error.hpp"

int main(int argc, char ** argv)
{


	try {

    udp_client client ("127.0.0.1", 5684);

    client.send((const uint8_t *)"HELLO SERVER", sizeof("HELLO SERVER") - 1);
    
    client.receive();
    
    std::cout << (const char *)client.get_buffer() << std::endl;

    client.send((const uint8_t *)"ACK", sizeof("ACK") - 1);

	}
	catch(udp_error &e) {
		std::cerr << "CLIENT: was cought the error from exception # " << e.get_code() << std::endl;
	}

    return EXIT_SUCCESS;
}