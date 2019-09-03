#ifndef UDP_CLIENT_HPP
#define UDP_CLIENT_HPP
#include <string>
#include <cstdint>
#include <cstdbool>
#include "udp_error.hpp"


enum {
    UDP_CLIENT_MAX_BUFFER_SIZE = 1024
};

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

};




#endif//UDP_CLIENT_HPP