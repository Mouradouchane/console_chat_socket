
#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif

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

#pragma comment(lib, "Ws2_32.lib") // look for Ws2_32.lib and link it 

const size_t MAX_BUFFER_SIZE = 512;
const size_t MAX_CLIENTS = 4;

std::string SERVER_NAME = "localhost";
std::string SERVER_PORT = "27015";

std::string server_log = "<SERVER> ";
std::string client_log = "<CLIENT> ";

struct addrinfo* result = NULL;
struct addrinfo* ptr = NULL;
struct addrinfo  hints;


int main(){

    // windows socket api data structer
    WSADATA wsaData;

    if (!win_socket_startup(wsaData)) {
        user_log(server_log, "socket startup successed", LOG_HINT);
    }
    else return WSAGetLastError();

    if (!setup_info(hints, result, SERVER_NAME, SERVER_PORT)) {
        user_log(server_log, "setup info successed", LOG_HINT);
    }
    else return WSAGetLastError();

    // server socket for listening
    SOCKET listen_socket = create_socket( result );

    log_socket_info(result , SERVER_NAME , SERVER_PORT );

    if (listen_socket != INVALID_SOCKET) {
        user_log(server_log , "listening to socket start successfuly" , LOG_WORK );
    }
    else return WSAGetLastError();

    if (bind_socket(listen_socket, result) != SOCKET_ERROR) {
        user_log(server_log, "binding socket successed", LOG_WORK);
    }
    else return WSAGetLastError();
               
    if (start_listening(listen_socket, MAX_CLIENTS) != SOCKET_ERROR) {
        user_log(server_log, "waiting for clients ...", LOG_WARN);
    }
    else return WSAGetLastError();

    // client socket for communication
    SOCKET client = INVALID_SOCKET ; client = waiting_for_client(listen_socket);

    if (client != INVALID_SOCKET) {
        user_log(server_log, "connection with client started", LOG_HINT);
    }
    else return WSAGetLastError();

    int recive_result = 0;
    int send_result   = 0;

    char recive_buffer [512]; clean_buffer(recive_buffer, MAX_BUFFER_SIZE);
    char send_buffer[512] = { "session start\0"};

    std::cout << "===============================================================\n";
    std::cout << "======================== SESSION START ========================\n";
    std::cout << "===============================================================\n";

    /*
        "send/recive" cycle
    */ 

    send_result = send_to(client, send_buffer , MAX_BUFFER_SIZE);
    do {

        recive_result = recive_from( client , recive_buffer , MAX_BUFFER_SIZE );
                           
        // if client send data
        if (recive_result > 0) {

            log_buffer( client_log , recive_buffer , MAX_BUFFER_SIZE );
            clean_buffer( recive_buffer , MAX_BUFFER_SIZE );

            // send message 

            std::cout << server_log;
            
            size_t i = 0;
            for ( ; i < MAX_BUFFER_SIZE ; i++ ) {
                send_buffer[i] = getchar();
                if (send_buffer[i] == ';') break;
            }
            send_buffer[i] = '\0';

            send_result = send_to( client , send_buffer , i );
            clean_buffer(send_buffer, MAX_BUFFER_SIZE);

        }

        Sleep(100);

    } while ( recive_result > 0 );

    // when the connection with the client come to end !
    int shut_down_result = shut_down_connection(client , SD_SEND);

    system("pause");
    return 0;
}
