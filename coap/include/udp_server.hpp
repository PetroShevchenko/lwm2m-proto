#ifndef UDP_SERVER_HPP
#define UDP_SERVER_HPP
#include <string>
#include <cstdint>
#include <cstdbool>
#include "udp_error.hpp"


enum {
    UDP_SERVER_MAX_BUFFER_SIZE = 1024
};

class udp_server {
    public:
        udp_server(std::string _addr, int _port);
        ~udp_server();
       void run();

    protected:
    virtual void step();

    udp_error error;    
    int sock;
    int port;
    std::string addr;
    struct sockaddr_in * client_address;
    struct sockaddr_in * server_address;
    //struct addrinfo * addrinfo;
    uint8_t * buffer;
    size_t length;
    int state;
    bool do_receive;
    bool do_send;
    bool do_stop;
    unsigned char receive_timeout;

    private:
    inline void client_hello_receive_step();
    inline void client_hello_ack_step();
    inline void server_hello_send_step();
    inline void server_hello_ack_step(); 
};


#endif//UDP_SERVER__HPP
