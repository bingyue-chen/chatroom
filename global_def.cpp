#include "global_def.h"

string char_to_string(char* p){
    string s="";
    while(*p){
        s += *p;
        p++;
    }
    return s;
}

int char_to_int(char* p){
    int n = 0;
    while(*p >= '0' && *p <= '9'){
        n = n*10 + *p - '0';
        p++;
    }
    return n;
}

string int_to_string(int n){
    string s="";
    stringstream ss;

    ss << n;
    ss >> s;

    return s;
}

char* int_to_char(int n, char* p){
    sprintf(p,"%d",n);
    return p;
}

int string_to_int(string s){
    int n = 0;
    stringstream ss;

    ss << s;
    ss >> n;

    return n;
}

string get_ip(struct sockaddr_in* addr){

    char ip[INET_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET, &((*addr).sin_addr), ip, INET_ADDRSTRLEN);

    return char_to_string(ip);
}

string get_ip(int fd){

    struct sockaddr_in addr;
    socklen_t addr_szie = sizeof(addr);
    int res = getpeername(fd, (struct sockaddr *)&addr, &addr_szie);

    return get_ip(&addr);
}

int get_port(struct sockaddr_in* addr){
    return ntohs((*addr).sin_port);
}

string get_color(int i){

    switch(i){
        case COLOR_RESET:     return "\x1b[0m";
        case COLOR_RED:       return "\x1b[31m";
        case COLOR_GREEN:     return "\x1b[32m";
        case COLOR_YELLOW:    return "\x1b[33m";
        case COLOR_BLUE:      return "\x1b[34m";
        case COLOR_MAGENTA:   return "\x1b[35m";
        case COLOR_CYAN:      return "\x1b[36m";
        case COLOR_B_RED:     return "\x1b[1;31m";
        case COLOR_B_GREEN:   return "\x1b[1;32m";
        case COLOR_B_YELLOW:  return "\x1b[1;33m";
        case COLOR_B_BLUE:    return "\x1b[1;34m";
        case COLOR_B_MAGENTA: return "\x1b[1;35m";
        case COLOR_B_CYAN:    return "\x1b[1;36m";
    }
}
