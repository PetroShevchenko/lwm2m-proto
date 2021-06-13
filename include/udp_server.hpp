#ifndef UDP_SERVER_HPP
#define UDP_SERVER_HPP
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
    bool is_encripted;

#ifdef USE_WOLFSSL

    WOLFSSL_CTX* ctx;
    WOLFSSL * ssl;

    std::string caCertLoc;
    std::string servCertLoc;
    std::string servKeyLoc;
#endif

    private:
#ifdef USE_WOLFSSL
    inline void dtls_client_hello_receive_step();
    inline void dtls_connect_step();
    inline void dtls_handshake_step();
    inline void dtls_error_handle_step();
#endif        
    inline void client_hello_receive_step();
    inline void client_hello_ack_step();
    inline void server_hello_send_step();
    inline void server_hello_ack_step();    

};


#endif//UDP_SERVER__HPP
