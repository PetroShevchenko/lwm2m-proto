#include "udp_server.hpp"
#include <iostream>
#include <cstdio> 
#include <cstdlib> 
#include <unistd.h> 
#include <cstring>

#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>

#include <sys/select.h>
#include <sys/time.h>
 

enum { 
#ifdef USE_WOLFSSL
    UDP_SERVER_DTLS_CLIENT_HELLO_RECEIVE_STATE,                                                                                                                                                                                                                                                                          
    UDP_SERVER_DTLS_CONNECT_STATE,                                                                                                                                                                                                                                                                                  
    UDP_SERVER_DTLS_HANDSHAKE_STATE,
    UDP_SERVER_DTLS_ERROR_HADLE_STATE,
#endif                                                                                                                                                                                                                                                                                                                 
    UDP_SERVER_CLIENT_HELLO_RECEIVE_STATE,                                                                                                                                                                                                                                                                          
    UDP_SERVER_CLIENT_HELLO_ACK_STATE,                                                                                                                                                                                                                                                                                  
    UDP_SERVER_SERVER_HELLO_SEND_STATE,                                                                                                                                                                                                                                                                                 
    UDP_SERVER_SERVER_HELLO_ACK_STATE                                                                                                                                                                                                                                                                                   
 };         

udp_server::udp_server(std::string _addr, int _port):
    addr(_addr), port(_port), length(0), state(0), do_receive(false), do_send(false), do_stop(false), receive_timeout(0)                                                                                                                                                                  
{                                                                                                                                                                                                                                                                                                                   
    int return_code;   
    const int on = 1;
    const socklen_t len = sizeof(int);
    is_encripted = false;

#ifdef USE_WOLFSSL

    ctx = NULL;
    ssl = NULL;
    caCertLoc = "../certs/ca-cert.pem";
    servCertLoc = "../certs/server-cert.pem";
    servKeyLoc = "../certs/server-key.pem";

    wolfSSL_Init();
    wolfSSL_Debugging_ON();

    if ((ctx = wolfSSL_CTX_new(wolfDTLSv1_2_server_method())) == NULL) {
        error.set_code(UDP_ERROR_WOLFSSL_NEW_CTX);                                                                                                                                                                                                                                                                    
        throw error;
    }

    if (wolfSSL_CTX_load_verify_locations(ctx,caCertLoc.c_str(),0) != SSL_SUCCESS) {
        error.set_code(UDP_ERROR_WOLFSSL_CERTS_LOCATION);                                                                                                                                                                                                                                                                    
        throw error;
    }

    if (wolfSSL_CTX_use_certificate_file(ctx, servCertLoc.c_str(), SSL_FILETYPE_PEM) != SSL_SUCCESS) {
        error.set_code(UDP_ERROR_WOLFSSL_CERTS_LOCATION);                                                                                                                                                                                                                                                                    
        throw error;
    }

    if (wolfSSL_CTX_use_PrivateKey_file(ctx, servKeyLoc.c_str(), SSL_FILETYPE_PEM) != SSL_SUCCESS) {
        error.set_code(UDP_ERROR_WOLFSSL_CERTS_LOCATION);                                                                                                                                                                                                                                                                    
        throw error;
    }

#endif                                                                                                                                                                                                                                                                                                 

    server_address = new struct sockaddr_in;
    client_address = new struct sockaddr_in;    

    buffer = new uint8_t [UDP_SERVER_MAX_BUFFER_SIZE];                                                                                                                                                                                                                                                              

    sock = socket (AF_INET, SOCK_DGRAM, 0);                                                                                                                                                                                                                                                                       

    if (sock < 0) {                                                                                                                                                                                                                                                                                               
        error.set_code(UDP_ERROR_CREATE_SOCKET);                                                                                                                                                                                                                                                                    
        throw error;                                                                                                                                                                                                                                                                                                
    }               
  
    memset(server_address, 0 , sizeof(sockaddr_in)); 
    memset(client_address, 0 , sizeof(sockaddr_in)); 

 
    server_address->sin_family = AF_INET;                                                                                                                                                                                                                                                                                  
    server_address->sin_addr.s_addr = INADDR_ANY;                                                                                                                                                                                                                                                                          
    server_address->sin_port = htons(port);   

    return_code = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, len);
    if (return_code < 0) {
        error.set_code(UDP_ERROR_SET_SOCK_OPT);                                                                                                                                                                                                                                                                    
        throw error; 
    }

    return_code =  bind (sock, (const struct sockaddr *)server_address, sizeof(*server_address));                                                                                                                                                                                                                             
    if (return_code < 0) {                                                                                                                                                                                                                                                                                          
        error.set_code(UDP_ERROR_BIND);                                                                                                                                                                                                                                                                             
        throw error;                                                                                                                                                                                                                                                                                                
    }                                                                                                                                                                                                                                                                                                               
}


udp_server::~udp_server()                                                                                                                                                                                                                                                                                               
{                                                                                                                                                                                                                                                                                                                   
     delete client_address;                                                                                                                                                                                                                                                                                             
     delete server_address;                                                                                                                                                                                                                                                                                             
     delete [] buffer;                                                                                                                                                                                                                                                                                       
}            



void udp_server::run()
{
    ssize_t received;
    ssize_t sent;
    int return_code;
    fd_set read_descriptors;
    struct timeval tv;
    static socklen_t address_length = 0;

    static struct sockaddr_storage peer_addr;
 

    while (!do_stop)
    {
        step();
  
        if (do_receive) {

            FD_ZERO (&read_descriptors);
            //FD_SET (0, &read_descriptors); // watch stdin (fd 0) to see when it has input
            FD_SET (sock, &read_descriptors); // watch socket descriptor to receive data from a client

            tv.tv_sec = receive_timeout;
            tv.tv_usec = 0;

            return_code = select (FD_SETSIZE, &read_descriptors, NULL, NULL, &tv);

            std::cout << "return_code is " << return_code << std::endl;

            if (return_code == 0) {
                std::cerr << " receive timeout is over" << std::endl;
                continue;
            }
            if (return_code == -1) {
                std::cerr << "the error happened: select " << std::endl;
                continue; 
            }
/*            if (FD_ISSET(0, &read_descriptors)) { //was entered something from stdin
                std::cout << "was entered something from stdin" << std::endl;
                char c;
                while (std::cin.get(c))
                    std::cout << c;
            }*/
            if (FD_ISSET(sock, &read_descriptors)) { //was received something from a socket
                
                if (!is_encripted) { 
                    
                    address_length = sizeof(*client_address);

                    received =  recvfrom (sock, buffer, UDP_SERVER_MAX_BUFFER_SIZE, MSG_WAITALL,
                        (struct sockaddr *) client_address, &address_length);

                    buffer[length] = 0;

                    std::cout << "received message : " << (const char *)buffer << std::endl; 
                    std::cout <<  "sin_family = " << client_address->sin_family << std::endl;
                    std::cout <<  "sin_port = " << client_address->sin_port << std::endl;
                    std::cout <<  "sin_addr = " << client_address->sin_addr.s_addr << std::endl;

                    if (received != length) {
                         std::cerr << "Receives and expected data is not compared" << std::endl;
                         std::cerr << "received = " << received << "length = " << length << std::endl;
                    }                

                    std::cout << "Packet has been received" << std::endl;
                }
                else {
#ifdef USE_WOLFSSL
                    received = wolfSSL_read(ssl, buffer, UDP_SERVER_MAX_BUFFER_SIZE);

                    if (received < 0) {
                        std::cerr << "Error happaned during receving" << std::endl;
                        state = UDP_SERVER_DTLS_ERROR_HADLE_STATE;
                        do_send = false;
                    }
                    else {
                        length = received;
                        std::cout << "Encripted packet has been received" << std::endl;       
                    }
#endif                   
                }
            }
        }
        if (do_send) {

            if (!is_encripted) { 
                sent =  sendto (sock, (const char *)buffer, (size_t)length, MSG_CONFIRM ,(const struct sockaddr *) client_address, sizeof(*client_address));
                std::cout << "errno =" << errno;

                if (sent != length) {
                    std::cerr << "the buffer was not sent completly " << std::endl;
                    std::cerr << "sent = " << sent << "length = " << length << std::endl;
                }
                std::cout << "Packet has been sent" << std::endl;
            }
            else {
#ifdef USE_WOLFSSL                
                sent = wolfSSL_write(ssl, buffer, length);

                if (sent < 0) {
                    std::cerr << "Error happaned during transmitting" << std::endl;
                    state = UDP_SERVER_DTLS_ERROR_HADLE_STATE;
                }
                else {
                    std::cout << "Encripted packet has been sent" << std::endl;
                }
#endif                
            }
        }   
    }
}

void udp_server::step()
{
    switch (state)
    {
#ifdef USE_WOLFSSL
        case UDP_SERVER_DTLS_CLIENT_HELLO_RECEIVE_STATE:
            dtls_client_hello_receive_step();
            break;
        case UDP_SERVER_DTLS_CONNECT_STATE:
            dtls_connect_step();
            break;
        case UDP_SERVER_DTLS_HANDSHAKE_STATE:
            dtls_handshake_step();
            break;
        case UDP_SERVER_DTLS_ERROR_HADLE_STATE:
            dtls_error_handle_step();
#endif
        case UDP_SERVER_CLIENT_HELLO_RECEIVE_STATE:
            client_hello_receive_step();
            break;
        case UDP_SERVER_CLIENT_HELLO_ACK_STATE:
            client_hello_ack_step();
            break;
        case UDP_SERVER_SERVER_HELLO_SEND_STATE:
            server_hello_send_step();
            break;
        case UDP_SERVER_SERVER_HELLO_ACK_STATE:
            server_hello_ack_step();
            break;
        default:
            state = 0;
            break;    
    }
}

#ifdef USE_WOLFSSL

inline void udp_server::dtls_client_hello_receive_step()
{
    receive_timeout = 15;

    do_receive = true;
    do_send = false;

    std::cout << "dtls_client_hello_receive_step" << std::endl;
    ++state;
}

inline void udp_server::dtls_connect_step()
{
    if (connect (sock, (const struct sockaddr *)client_address, sizeof(*client_address)) != 0) {
        std::cout << "UDP connect failed" << std::endl;
        state = UDP_SERVER_DTLS_ERROR_HADLE_STATE;
    }
    else {
        std::cout << "dtls_connect_step()" << std::endl;
        ++state;
    }
    do_receive = false;
    do_send = false;  
}

inline void udp_server::dtls_handshake_step()
{
    std::cout << "dtls_handshake_step()" << std::endl;

    ssl = wolfSSL_new(ctx);
    if (ssl == NULL) {
        state = UDP_SERVER_DTLS_ERROR_HADLE_STATE;
        return;
    }

    wolfSSL_set_fd(ssl, sock);

    if (wolfSSL_accept(ssl) != SSL_SUCCESS) {
        state = UDP_SERVER_DTLS_ERROR_HADLE_STATE;
        return;        
    }
    is_encripted = true;
    state = UDP_SERVER_CLIENT_HELLO_RECEIVE_STATE; 
}

inline void udp_server::dtls_error_handle_step()
{
    int error = wolfSSL_get_error(ssl, 0);

    std::cout << "error = " << error << wolfSSL_ERR_reason_error_string(error) << std::endl;

    do_stop = true;
    std::cout << "dtls_error_handle_step()" << std::endl;
}
#endif

inline void udp_server::client_hello_receive_step()
{
    const std::string client_hello_message = "HELLO SERVER";
    
    length = (size_t)client_hello_message.length();

    receive_timeout = 5;

    do_receive = true;
    do_send = false;

    std::cout << "client_hello_receive_step()" << std::endl;
     ++state; 
}
inline void udp_server::client_hello_ack_step()
{
    const std::string client_ack_message = "ACK";
    std::memcpy (buffer, client_ack_message.c_str(), client_ack_message.length());
    length = (size_t)client_ack_message.length();

    buffer[length] = 0;
    std::cout << "buffer : " << (char *)buffer << std::endl;
    std::cout << "buffer length : " << length << std::endl;


    do_send = true;
    do_receive = false;
    
    std::cout << "client_hello_ack_step()" << std::endl;
    ++state;
}

inline void udp_server::server_hello_send_step()
{
    do_send = false;
    do_receive = false;
    std::cout << "server_hello_send_step()" << std::endl;
    ++state;
}

inline void udp_server::server_hello_ack_step()
{
    std::cout << "server_hello_ack_step()" << std::endl;

    do_stop = true;
    state = 0;
}
