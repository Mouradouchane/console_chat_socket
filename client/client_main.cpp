
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

struct addrinfo* result = NULL;
struct addrinfo* ptr = NULL;
struct addrinfo  hints;

std::string server_log = "<SERVER> ";
std::string client_log = "<CLIENT>";

std::string SERVER_NAME = "localhost";
std::string SERVER_PORT = "27015";
size_t max_buffer_size = 512;

int main(){

    // socket data structer
    WSADATA wsaData; 

    if ( !win_socket_startup(wsaData) ) {
        user_log( client_log , "socket startup successed", LOG_HINT);
    }
    else return WSAGetLastError();
    
    if ( !setup_info(hints, result, SERVER_NAME, SERVER_PORT) ) {
        user_log( client_log , "setup info successed", LOG_HINT);
    }
    else return WSAGetLastError();

    log_socket_info( result , SERVER_NAME , SERVER_PORT );

    // create socket object 
    SOCKET ConnectSocket = INVALID_SOCKET;

    // try -> to open socket based on the info we have in object 
    ConnectSocket = create_socket( result );

    if (ConnectSocket != INVALID_SOCKET) {
        user_log(client_log, "socket created successfully", LOG_WORK);
    }
    else {
        freeaddrinfo(result);
        WSACleanup();

        return WSAGetLastError();
    }

    // try to connect to the server
    int try_connect_result = connect_to(ConnectSocket , result);

    // try to connect until you success
    while ( try_connect_result == SOCKET_ERROR || ConnectSocket == INVALID_SOCKET ) {

        user_log(client_log, "failed to connect with the server [TRY AGAIN]", LOG_WARN);
        try_connect_result = connect_to(ConnectSocket, result);

        Sleep(250);
    }

    user_log(client_log, "connection with the server established", LOG_HINT);

    /*
        start send/recive cycle 
    */
    char send_buffer   [512] = { "hi" };
    char recive_buffer [512]; clean_buffer( recive_buffer, max_buffer_size );

    int send_result   = 0;
    int revice_result = 0;

    std::cout << "===============================================================\n";
    std::cout << "======================== SESSION START ========================\n";
    std::cout << "===============================================================\n";

    do {

        revice_result = recive_from(ConnectSocket, recive_buffer, max_buffer_size);

        if (revice_result > 0) {

            log_buffer(server_log, recive_buffer , max_buffer_size);
            clean_buffer(recive_buffer, max_buffer_size);

            std::cout << client_log;
            
            size_t i = 0;
            for ( ; i < max_buffer_size ; i++) {
                send_buffer[i] = getchar();
                if (send_buffer[i] == ';') break;
            }
            send_buffer[i] = '\0';

            std::cout << '\n';

            send_result = send_to(ConnectSocket, send_buffer , i);
            clean_buffer(send_buffer, max_buffer_size);


            if (send_result < 1) {
                user_log(client_log, "connection with server closed", LOG_WARN);
                break;
            }

        }

        Sleep(100);

    } while (revice_result > 0);

    send_result = shut_down_connection(ConnectSocket, SD_SEND);

    if (send_result > 0) {
        user_log(client_log, "connection closed successfully", LOG_WARN);
    }
    else user_log(client_log, "error while trying to close connection", LOG_ERROR);
    

    closesocket(ConnectSocket);
    WSACleanup();

    system("pause");
    return 0;

}
