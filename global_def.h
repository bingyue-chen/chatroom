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
#define IS_ONLINE -1
#define FAILED_NAME -2
#define FAILED -3

#define SEND_SUCCESS 0
#define USER_OFFLINE 1
#define USER_NOT_EXIST 2

using namespace std;

string char_to_string(char* p);
int    char_to_int(char* p);

string int_to_string(int n);
char*   int_to_char(int n, char* p);

int string_to_int(string s);

string get_ip(struct sockaddr_in* addr);
string get_ip(int fd);
int    get_port(struct sockaddr_in* addr);
