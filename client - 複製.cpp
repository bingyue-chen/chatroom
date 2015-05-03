#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <pthread.h>
#include <cstring>

#include "client_socket.h"
#include "global_def.h"

using namespace std;

static char socket_buffer[socket_buffer_size+1];
static bool is_login;

static void showhelpmsg();
static void read_command(vector<string>& command);
static bool doConnect(string host, string service, int &socket);
static void doDisconnect(int &socket);
static int doLogin(int socket, string name);
static int doSend(int socket, string name, string message);

int main(){

    string host, service, s;
    int socket;
    vector<string> command;
    is_login = false;

    showhelpmsg();

    while(1){
        cout << endl;
        read_command(command);
        /*for(unsigned int i = 0; i < command.size(); i++){
            cout << "command " << i << " :" << command[i] << ":" <<endl;
        }*/
        if(command.size() == 0) continue;
        if(command[0] == "help") showhelpmsg();
        else if(command[0] == "connect"){
            if(command.size() < 4 ){
                cout << "error command" << endl;
                cout << "connect [host] [port]    [name]" << endl;
                continue;
            }

            if(command[3].size() < 5){
                cout << "name is should be long than 4" << endl;
                continue;
            }

            cout << "connect..."  << endl;
            if( doConnect(command[1], command[2], socket) ){
                cout << "connect success" << endl;
                cout << "login..." << endl;
                int n = doLogin(socket, command[3]);
                if(n == FAILED){
                    cout << "some thing failed" << endl;
                    close(socket);
                }
                else if(n == FAILED_NAME){
                    cout << "invalid name" << endl;
                    close(socket);
                }
                else{
                    is_login = true;
                    cout << "login success" << endl;
                }
            }
            else{
                cout << "connect failed" << endl;
            }
        }
        else if(command[0] == "chat"){
            if(command.size() < 3 ){
                cout << "error command" << endl;
                cout << "chat    [name] [name]... [\"message\"]" << endl;
                continue;
            }
            if(!is_login){
                cout << "in disconnect state, please connect" << endl;
                continue;
            }
            string message = command[command.size()-1];
            for(unsigned int i = 1; i < command.size()-1; i++){
                int n = doSend(socket, command[i], message);
                if(n < 0 ){
                    write(socket, "bye", socket_buffer_size);
                    close(socket);
                    is_login = false;
                    cout << "failed to transmit, disconnect, please connect again";
                    break;
                }
                else if(n == USER_NOT_EXIST) cout << command[i] << " not a member of this chatroom" << endl;
                else if(n == USER_OFFLINE) cout << command[i] << " is offline" << endl;
            }
        }
        else if(command[0] == "bye"){
             if(!is_login){
                cout << "in disconnect state, please connect" << endl;
                continue;
            }
            is_login = false;
            write(socket, command[0].c_str(), socket_buffer_size);
            int n;
            while(1){
                n = read(socket, socket_buffer, socket_buffer_size);
                if(n < 0) break;
                socket_buffer[n] = '\0';
                n = strlen(socket_buffer);
                if(n >= 5) cout << socket_buffer << endl;
                if(n < 5) break;
            }
            close(socket);
            cout << "disconnect" << endl;
            break;
        }
        else{
            cout << "invalid command" << endl;
        }
    }

    return 0;
}

static void showhelpmsg(){
    cout << "connect [host] [port]    [name]" << endl;
    cout << "chat    [name] [name]... [\"message\"]" << endl;
    cout << "bye" << endl;
    cout << "help" << endl << endl;
}

static void read_command(vector<string>& command){

    string s;
    char c;

    getline(cin, s);

    size_t found = s.find_first_of(" \"");
    command.clear();

    while( found != std::string::npos ){
        if(found > 0)
        command.push_back(s.substr(0, found));
        c = s[found];
        s = s.substr(found+1, s.size()- found);
        if(c == '\"')
            found = s.find_first_of("\"");
        else
            found = s.find_first_of(" \"");
    }

    if(s != "")
        command.push_back(s);
}

static bool doConnect(string host, string service, int &socket){

    if((socket = connect_tcp(host.c_str(), service.c_str())) < 0) return false;
    return true;
}

static void doDisconnect(int &socket){
    close(socket);
};

static int doLogin(int socket, string name){
    int n;

    write(socket, name.c_str(), socket_buffer_size);
    while((n = read(socket, socket_buffer, socket_buffer_size)) == 0);

    if(n < 0) return FAILED;
    socket_buffer[n] = '\0';
    return char_to_int(socket_buffer);
}

static int doSend(int socket, string name, string message){
    int n;

    write(socket, (name+":"+message).c_str(), socket_buffer_size);

    while(1){
        n = read(socket, socket_buffer, socket_buffer_size);
        if(n < 0) break;
        socket_buffer[n] = '\0';
        n = strlen(socket_buffer);
        if(n >= 5) cout << socket_buffer << endl;
        if(n < 5) break;
    }

    if(n < 0) return FAILED;
    socket_buffer[n] = '\0';
    return char_to_int(socket_buffer);
}
