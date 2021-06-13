#ifndef UDP_CLIENT_HPP
#define UDP_CLIENT_HPP
#include <string>
#include <cstdint>
#include <cstdbool>
#include "udp_error.hpp"
#include "config.h"
#ifdef USE_WOLFSSL
#include <wolfssl/options.h>
#include <wolfssl/ssl.h>
#include <netdb.h>
#include <string>
#endif

class udp_client {
    public:
        udp_client(std::string _addr, int _port);
        ~udp_client();
	    void send(const uint8_t * data, const size_t length);
    	void receive();

    	uint8_t * get_buffer() const {
    		return buffer;
    	}
    	size_t get_length() const
    	{
    		return length;
    	}


    protected:

    udp_error error;    
    int sock;
    int port;
    std::string addr;

    struct sockaddr_in * server_address;

    uint8_t * buffer;
    size_t length;

#ifdef USE_WOLFSSL

    std::string cert_array; 
    const char * certs; 

    WOLFSSL_CTX* ctx;
    WOLFSSL * ssl;
#endif    

};




#endif//UDP_CLIENT_HPP