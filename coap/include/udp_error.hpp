#ifndef UDP_ERROR_HPP
#define UDP_ERROR_HPP
#include <iostream>
#include <vector>

enum {
    UDP_ERROR_CREATE_SOCKET = 0,
    UDP_ERROR_BIND,
    UDP_ERROR_NUMBER
};
#define UDP_ERROR_MIN   UDP_ERROR_CREATE_SOCKET
#define UDP_ERROR_MAX   UDP_ERROR_BIND

enum {
    UDP_ERROR_INVAL = -1,
    UDP_ERROR_OK = 0
};

class udp_error {
    public:
        udp_error()
        {
        }
        ~udp_error()
        {
        }
        int set_code(int error_code) 
        {
            if ( (error_code < UDP_ERROR_MIN) || (error_code > UDP_ERROR_MAX)) return UDP_ERROR_INVAL;
            code = error_code;
            return UDP_ERROR_OK;
        }
        void show_message()
        {
            if (code > messages.max) return;
            std::cout << messages.text[code];
        }
    
    protected:

    int code;

    struct {
        const int max = 3;
        const std::vector <std::string> text = {
            "No any errors",
            "Can not create a socket ",
            "Can not bind IP address and port"
        }; 
    } messages;

};




#endif//UDP_EROR_HPP
