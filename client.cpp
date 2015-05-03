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
static bool is_stop;
static pthread_t thread;
static pthread_mutex_t lock;
static int client_socket;
static int server_flag;

static void showhelpmsg();
static void read_command(vector<string>& command);
static bool doConnect(string host, string service);
static int doLogin(string name);
static int doSend(string name, string message);
static void *receive_messsage( void *argv );
static int read_server_flag();

int main(){

    string host, service, s;

    vector<string> command;
    is_login = is_stop = false;
    server_flag = -100;

     if ( pthread_mutex_init( &lock , NULL ) != 0 ){
        printf("\n mutex init failed\n");
        return 1;
    }

    if( pthread_create(&thread, NULL, &receive_messsage, 0) != 0 ){
        printf("thread can't creat\n");
        return 1;
    }

    showhelpmsg();

    while(1){
        cout << endl;
        read_command(command);
        /*for(unsigned int i = 0; i < command.size(); i++){
            cout << "command " << i << " :" << command[i] << ":" <<endl;
        }*/

        if(command.size() == 0);
        else if(command[0] == "help") showhelpmsg();
        else if(command[0] == "connect"){
            if(command.size() < 4 ){
                cout << "error command" << endl;
                cout << "connect [host] [port]    [name]" << endl;
            }
            else if(command[3].size() < 5){
                cout << "name is should be long than 4" << endl;
            }
            else if(cout << "connect..."  << endl && doConnect(command[1], command[2]) ){
                cout << "connect success" << endl;
                cout << "login..." << endl;
                int n = doLogin(command[3]);
                if(n == FAILED){
                    cout << "some thing failed" << endl;
                    close(client_socket);
                }
                else if(n == FAILED_NAME){
                    cout << "invalid name" << endl;
                    close(client_socket);
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
            }
            else if(!is_login){
                cout << "in disconnect state, please connect" << endl;
            }
            else{
                string message = command[command.size()-1];
                for(unsigned int i = 1; i < command.size()-1; i++){
                    int n = doSend(command[i], message);
                    if(n < 0 ){
                        int w = write(client_socket, "bye", socket_buffer_size);
                        n = read_server_flag();
                        close(client_socket);
                        is_login = false;
                        cout << "failed to transmit, disconnect, please connect again";
                        break;
                    }
                    else if(n == USER_NOT_EXIST) cout << command[i] << " not a member of this chatroom" << endl;
                    else if(n == USER_OFFLINE) cout << command[i] << " is offline" << endl;
                }
            }
        }
        else if(command[0] == "bye"){
            if(!is_login){
                cout << "in disconnect state, please connect" << endl;
            }
            else{
                int w = write(client_socket, command[0].c_str(), socket_buffer_size);
                int n;
                n = read_server_flag();
                is_login = false;
                close(client_socket);
                cout << "disconnect" << endl;
            }
        }
        else{
            cout << "invalid command" << endl;
        }
    }

    is_stop = true;
    pthread_join(thread, NULL);
    pthread_mutex_destroy( &lock );

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

static bool doConnect(string host, string service){

    if((client_socket = connect_tcp(host.c_str(), service.c_str())) < 0) return false;
    return true;
}

static int doLogin(string name){
    int n;

    int w = write(client_socket, name.c_str(), socket_buffer_size);
    while((n = read(client_socket, socket_buffer, socket_buffer_size)) == 0);

    if(n < 0) return FAILED;
    socket_buffer[n] = '\0';
    return char_to_int(socket_buffer);
}

static int doSend(string name, string message){
    int w = write(client_socket, (name+":"+message).c_str(), socket_buffer_size);
    return read_server_flag();
}

static void* receive_messsage(void* argv){

    int n;

    while(!is_stop){
        if(is_login && server_flag == -100){
            n = read(client_socket, socket_buffer, socket_buffer_size);
            if(n > 0) {
                socket_buffer[n] = '\0';
                n = strlen(socket_buffer);
                if(n > 5) cout << socket_buffer << endl;
                else{
                    pthread_mutex_lock( &lock );
                        server_flag = char_to_int(socket_buffer);
                    pthread_mutex_unlock( &lock );
                }
            }
        }
        sleep(1);
    }

}

static int read_server_flag(){

    int n = -100;

    while(server_flag == -100) sleep(1);
    pthread_mutex_lock( &lock );
        n = server_flag;
        server_flag = -100;
    pthread_mutex_unlock( &lock );

    return n;
}
