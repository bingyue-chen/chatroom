#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstdio>
#include <cstdlib>

#define socket_buffer_size 256

#define NEW_USER 0
#define OLD_USER 1
#define IS_ONLINE 2
#define FAILED_NAME 3
#define FAILED 4

#define SEND_SUCCESS 0
#define USER_OFFLINE 1
#define USER_NOT_EXIST 2

#define COLOR_RED     1
#define COLOR_GREEN   2
#define COLOR_YELLOW  3
#define COLOR_BLUE    4
#define COLOR_MAGENTA 5
#define COLOR_CYAN    6
#define COLOR_RESET   0

#define COLOR_B_RED     7
#define COLOR_B_GREEN   8
#define COLOR_B_YELLOW  9
#define COLOR_B_CYAN    10
#define COLOR_B_MAGENTA 11
#define COLOR_B_BLUE    12

using namespace std;

string char_to_string(char* p);
int    char_to_int(char* p);

string int_to_string(int n);
char*   int_to_char(int n, char* p);

int string_to_int(string s);

string get_ip(struct sockaddr_in* addr);
string get_ip(int fd);
int    get_port(struct sockaddr_in* addr);
string get_color(int i);
