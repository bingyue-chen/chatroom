#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include <sstream>
#include <string.h>

#include "server_socket.h"
#include "usermanage.h"
#include "global_def.h"

#define QLEN 32

using namespace std;

static char socket_buffer[socket_buffer_size+1];
static Usermanage usermanager;

static bool doLogin(int fd, string ip, int port);
static bool doSend(int fd);
static void doExit(int fd, fd_set*);

int main(){

    struct sockaddr_in fsin;
    int msock;
    fd_set rfds, afds;
    unsigned int alen;
    int fd, nfds;

    msock = passive_TCP("8080", QLEN);
    nfds = getdtablesize();
    FD_ZERO(&afds);
    FD_SET(msock, &afds);

    while(1){
        memcpy(&rfds, &afds, sizeof(rfds));

        if(select(nfds, &rfds, (fd_set *)0, (fd_set *)0, (struct timeval *)0) < 0){
            printf("select error\n");
            exit(-1);
        }

        if(FD_ISSET(msock, &rfds)){
            int ssock;
            alen = sizeof(fsin);
            ssock = accept(msock, (struct sockaddr*)&fsin, &alen);
            if(doLogin(ssock, get_ip(&fsin), get_port(&fsin)))
                FD_SET(ssock, &afds);
            usermanager.show();
        }

        for(fd = 0; fd < nfds; fd++){
            if(fd != msock && FD_ISSET(fd, &rfds)){
                if(!doSend(fd)) doExit(fd, &afds);
                usermanager.show();
            }
        }
    }

    return 0;
}

static bool doLogin(int fd, string ip, int port){

    int n;

    /*read name*/
    while((n = read(fd, socket_buffer, socket_buffer_size)) == 0);
    if(n < 0)
        return false;

    socket_buffer[n] = '\0';
    string name = char_to_string(socket_buffer);

    /* [0 => new user, 1 => old user, -1 => is online, -2 => same name, -3 => failed] */
    n = usermanager.add_user(User(fd, name, ip, port));

    int w = write(fd, int_to_string(n).c_str(), socket_buffer_size);

    if(n == NEW_USER)
        usermanager.broadcast("<server> : New user " + name + " is login !\n");
    else if(n == OLD_USER)
        usermanager.broadcast("<server> : " + name + " is online !\n");
    else
        close(fd);

    return (n == NEW_USER || n == OLD_USER);
}

static bool doSend(int fd){

    int n;

    while((n = read(fd, socket_buffer, socket_buffer_size)) == 0);
    if(n < 0)
        return false;

    socket_buffer[n] = '\0';
    string input = char_to_string(socket_buffer);

    if(input == "bye") return false;

    size_t found = input.find_first_of(":");
    string name    = input.substr(0, found);
    string message = input.substr(found+1, input.size() - found);

    n = usermanager.send_msg(get_ip(fd), message, name);

    int w;

    w = write(fd, int_to_string(n).c_str(), socket_buffer_size);

    return true;
}

void doExit(int fd, fd_set* fs){
    usermanager.user_exit(get_ip(fd));
    int w = write(fd, "bye", socket_buffer_size);
    close(fd);
    FD_CLR(fd, fs);
}
