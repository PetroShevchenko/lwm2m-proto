//udp_client.cpp
#include "udp_client.hpp"
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




udp_client::udp_client(std::string _addr, int _port):
    addr(_addr), port(_port), length(0)                                                                                                                                                                      
{                                                                                                                                                                                                                                                                                                                   
    int return_code;                                                                                                                                                                                                                                                                                                

    server_address = new struct sockaddr_in; 

    buffer = new uint8_t [UDP_CLIENT_MAX_BUFFER_SIZE];                                                                                                                                                                                                                                                              

    sock = socket (AF_INET, SOCK_DGRAM, 0);                                                                                                                                                                                                                                                                       

    if (sock < 0) {                                                                                                                                                                                                                                                                                               
        error.set_code(UDP_ERROR_CREATE_SOCKET);                                                                                                                                                                                                                                                                    
        throw error;                                                                                                                                                                                                                                                                                                
    }               
  
    memset(server_address, 0 , sizeof(sockaddr_in)); 
 
     server_address->sin_family = AF_INET;                                                                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                        
     server_address->sin_port = htons(port);

     inet_aton (_addr.c_str(), (in_addr*)&server_address->sin_addr.s_addr );                                                                                                                                                                                                                                                                              

}


udp_client::~udp_client()                                                                                                                                                                                                                                                                                               
{                                                                                                                                                                                                                                                                                                                   
	 close(sock);	                                                                                                                                                                                                                                                                                           
     delete server_address;                                                                                                                                                                                                                                                                                             
     delete [] buffer;                                                                                                                                                                                                                                                                                       
}    


void udp_client::send(const uint8_t * data, const size_t length)
{
	ssize_t sent;

	sent = sendto (sock, data, length, MSG_CONFIRM, (const struct sockaddr *) server_address, sizeof(*server_address));

	if (sent != length) {
        error.set_code(UDP_ERROR_SEND);                                                                                                                                                                                                                                                                    
        throw error; 
	} 
}        

void udp_client::receive()
{
	ssize_t received; 
	socklen_t address_length;
	struct sockaddr client_address;
	
	received = recvfrom (sock, (char *)buffer, UDP_CLIENT_MAX_BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &client_address,  &address_length);


	if (received == -1) {
        error.set_code(UDP_ERROR_RECEIVE);                                                                                                                                                                                                                                                                    
        throw error;	
	}

	length = (size_t)received; 
}
