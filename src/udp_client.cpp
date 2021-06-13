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
    server_address = new struct sockaddr_in; 

    buffer = new uint8_t [UDP_CLIENT_MAX_BUFFER_SIZE];
    
    memset(server_address, 0 , sizeof(sockaddr_in)); 
 
    server_address->sin_family = AF_INET;                                                                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                        
    server_address->sin_port = htons(port);

    inet_aton (_addr.c_str(), (in_addr*)&server_address->sin_addr.s_addr );                                                                                                                                                                                                                                                                                                                     
    
#ifdef USE_WOLFSSL 
    
    ctx = NULL;
    ssl = NULL;
    cert_array = "../certs/ca-cert.pem";
    certs = cert_array.c_str();
 
    wolfSSL_Init();
    wolfSSL_Debugging_ON(); 

    if ( (ctx = wolfSSL_CTX_new(wolfDTLSv1_2_client_method())) == NULL) {
        error.set_code(UDP_ERROR_WOLFSSL_NEW_CTX);                                                                                                                                                                                                                                                                    
        throw error;
    }
    if (wolfSSL_CTX_load_verify_locations(ctx, certs, 0) != SSL_SUCCESS) {
        error.set_code(UDP_ERROR_WOLFSSL_CERTS_LOCATION);                                                                                                                                                                                                                                                                    
        throw error;
    }

    ssl = wolfSSL_new(ctx);
    if (ssl == NULL) {
        error.set_code(UDP_ERROR_WOLFSSL_NEW_OBJECT);                                                                                                                                                                                                                                                                    
        throw error;
    }

    wolfSSL_dtls_set_peer(ssl, server_address, sizeof(*server_address));
#endif
>>>>>>> Stashed changes

    sock = socket (AF_INET, SOCK_DGRAM, 0);                                                                                                                                                                                                                                                                       

    if (sock < 0) {                                                                                                                                                                                                                                                                                               
        error.set_code(UDP_ERROR_CREATE_SOCKET);                                                                                                                                                                                                                                                                    
        throw error;                                                                                                                                                                                                                                                                                                
    } 

#ifdef USE_WOLFSSL    
    wolfSSL_set_fd (ssl, sock);
    if (wolfSSL_connect(ssl) != SSL_SUCCESS) { // DTLS client handshake
        error.set_code(UDP_ERROR_WOLFSSL_CONNECT);                                                                                                                                                                                                                                                                    
        throw error;   
    }
#endif
    
}


udp_client::~udp_client()                                                                                                                                                                                                                                                                                               
{
#ifdef USE_WOLF_SSL
    wolfSSL_shutdown(ssl);
    wolfSSL_free(ssl);
#endif
    close(sock);
#ifdef USE_WOLFSSL    
    wolfSSL_CTX_free(ctx);
    wolfSSL_Cleanup();    
#endif                                                                                                                                                                                                                                                                        
    delete server_address;                                                                                                                                                                                                                                                                                             
    delete [] buffer;                                                                                                                                                                                                                                                                                       
}    


void udp_client::send(const uint8_t * data, const size_t length)
{
	ssize_t sent;
#ifdef USE_WOLFSSL
    sent = wolfSSL_write(ssl, data, length);
#else
	sent = sendto (sock, data, length, MSG_CONFIRM, (const struct sockaddr *) server_address, sizeof(*server_address));
#endif
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
<<<<<<< Updated upstream
	
    memset (buffer, 0 , UDP_CLIENT_MAX_BUFFER_SIZE);

	received = recvfrom (sock, (char *)buffer, UDP_CLIENT_MAX_BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &client_address,  &address_length);
=======
>>>>>>> Stashed changes

#ifdef USE_WOLFSSL
    received = wolfSSL_read(ssl, buffer, UDP_CLIENT_MAX_BUFFER_SIZE);
#else	
	received = recvfrom (sock, (char *)buffer, UDP_CLIENT_MAX_BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &client_address,  &address_length);
#endif

	if (received == -1) {
        error.set_code(UDP_ERROR_RECEIVE);                                                                                                                                                                                                                                                                    
        throw error;	
	}

	length = (size_t)received; 
}
