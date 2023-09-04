
#include <string>

#define LOG_HINT 10
#define LOG_WARN 14
#define LOG_ERROR 12
#define LOG_WORK 13
#define LOG_DEF 15

HANDLE _std_console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

void setConsoleColor( short const& color ) {

	SetConsoleTextAttribute(_std_console_handle, color );

}

void user_log( std::string const& username , std::string const& message , short const& color ) {

	setConsoleColor(color);

	std::cout << username << " " << message << '\n';

	setConsoleColor(LOG_DEF);

}