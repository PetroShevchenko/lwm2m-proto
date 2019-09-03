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
    UDP_SERVER_CLIENT_HELLO_RECEIVE_STATE = 0,                                                                                                                                                                                                                                                                          
    UDP_SERVER_CLIENT_HELLO_ACK_STATE,                                                                                                                                                                                                                                                                                  
    UDP_SERVER_SERVER_HELLO_SEND_STATE,                                                                                                                                                                                                                                                                                 
    UDP_SERVER_SERVER_HELLO_ACK_STATE                                                                                                                                                                                                                                                                                   
 };         

udp_server::udp_server(std::string _addr, int _port):
    addr(_addr), port(_port), length(0), state(0), do_receive(false), do_send(false), do_stop(false), receive_timeout(0)                                                                                                                                                                  
{                                                                                                                                                                                                                                                                                                                   
    int return_code;                                                                                                                                                                                                                                                                                                

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
                 
                 received =  recvfrom (sock, buffer, UDP_SERVER_MAX_BUFFER_SIZE, MSG_WAITALL,
                    (struct sockaddr *) client_address, &address_length);

                 if (received != length) {
                     std::cerr << "Receives and expected data is not compared" << std::endl;
                     std::cerr << "received = " << received << "length = " << length << std::endl;
                 }

                std::cout << "Packet has been received" << std::endl;
            }
        }
        if (do_send) {

            std::cout << "sock = " << sock << std::endl;
            std::cout << "client_address = " << client_address << std::endl;
            std::cout << "address_length = " << address_length << std::endl;


            sent =  sendto (sock, (const char *)buffer, (size_t)length, MSG_CONFIRM ,(const struct sockaddr *) client_address, sizeof(*client_address));

            std::cout << "errno =" << errno;

            if (sent != length) {
                std::cerr << "the buffer was not sent completly " << std::endl;
                std::cerr << "sent = " << sent << "length = " << length << std::endl;
            }
            std::cout << "Packet has been sent" << std::endl;
        }   
    }
}

void udp_server::step()
{
    switch (state)
    {
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
