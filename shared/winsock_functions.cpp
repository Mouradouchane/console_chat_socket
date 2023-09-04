
#ifndef INCLUDE_WIN_SOCKET_STUFF

    #define INCLUDE_WIN_SOCKET_STUFF

    #include <iostream>
    #include <string>

    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <iphlpapi.h> // for ip helper api's
    #include <Windows.h>

    #include "../shared/logs.cpp"
    #include "../shared/winsock_functions.cpp"

#endif

void log_socket_info( addrinfo* result , std::string & address , std::string & port ) {

    char hostname[65]; gethostname(hostname, 64); hostname[64] = '\0';

    // log socket info
    std::cout << "hostname   : " << hostname << '\n';
    std::cout << "ip-address : " << address << '\n';
    std::cout << "port       : " << port << '\n';
    std::cout << "protocol   : " << result->ai_protocol << '\n';
    std::cout << "socktype   : " << result->ai_socktype << '\n';
    std::cout << "flags      : " << result->ai_flags << '\n';
    std::cout << "family     : " << result->ai_family << '\n';
}


// Initialize Winsock
int win_socket_startup( WSADATA & win_sock_data ) {

    return WSAStartup(MAKEWORD(2, 2), &win_sock_data);
}

int setup_info( addrinfo& addr_info_obj , addrinfo*& result_obj , std::string const& server_name , std::string const& server_port ) {

    // setup hints
    ZeroMemory(&addr_info_obj, sizeof(addr_info_obj));
    addr_info_obj.ai_family = AF_INET;
    addr_info_obj.ai_socktype = SOCK_STREAM;
    addr_info_obj.ai_protocol = IPPROTO_TCP;
    addr_info_obj.ai_flags = AI_PASSIVE;

    // get address info output
    return getaddrinfo(server_name.c_str(), server_port.c_str() , &addr_info_obj, &result_obj);
}

SOCKET create_socket(addrinfo* result_obj) {

    // create socket to listen to clinet's request's
    return socket(result_obj->ai_family, result_obj->ai_socktype, result_obj->ai_protocol);
}

int bind_socket(SOCKET& socket_obj, addrinfo*& result_obj) {

    // Setup the TCP listening socket
    int binding = bind(socket_obj, result_obj->ai_addr, (int)result_obj->ai_addrlen);

    if (binding == SOCKET_ERROR) {

        freeaddrinfo(result_obj);
        closesocket(socket_obj);

        WSACleanup();

    }
    else { // when socket binded successfully :)
        freeaddrinfo(result_obj);
    }

    return binding;
}


int start_listening(SOCKET& socket_object, size_t backlog_amount) {

    return listen(socket_object, (int)backlog_amount);
}


SOCKET waiting_for_client(SOCKET& listening_socket) {

    // that later for multi-client's :)
    /*
        // socket's for client's
        SOCKET clients_sockets[MAX_CLIENTS];

        for (size_t i = 0; i < MAX_CLIENTS; i++) {
            clients_sockets[i] = INVALID_SOCKET;
        }
    */

    SOCKET client_socket = INVALID_SOCKET;

    // keep listen/waiting for client request
    client_socket = accept(listening_socket, NULL, NULL);

    if (client_socket == INVALID_SOCKET) {

        closesocket(client_socket);
        WSACleanup();

    }

    return client_socket;
}


int connect_to( SOCKET & socket_obj , addrinfo* addr_object) {

    return connect(socket_obj, addr_object->ai_addr, addr_object->ai_addrlen);

}


int send_to(SOCKET& client_socket, char * buffer , size_t const& length , int flag = 0) {

    // send back "ok" to the client
    int send_result = send(client_socket, buffer , length , flag);

    if (send_result == SOCKET_ERROR) {
        closesocket(client_socket);
        WSACleanup();
    }

    return send_result;
}

int recive_from(SOCKET& client_socket, char * buffer , size_t const& length , int flag = 0) {

    // try to recive client request 

    return recv(client_socket, buffer, length, flag);
}


void log_buffer(std::string& user_log, char * buffer , size_t const& length ) {

    // log recived buffer

    setConsoleColor(LOG_WORK);
    std::cout << user_log << " ";
    setConsoleColor(LOG_DEF);

    for (size_t i = 0; i < length; i++) {
        if (buffer[i] == '\0') break;
        std::cout << buffer[i];
    }
    std::cout << '\n';

}

void clean_buffer(char * buffer, size_t const& size) {

    ZeroMemory(buffer, size);

}

int shut_down_connection(SOCKET& client_socket, int how_to = SD_SEND) {

    // try close connection
    return shutdown(client_socket, how_to);;
}