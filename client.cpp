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
static void show_color_choice();

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
                cout << get_color(COLOR_RED) + "error command" + get_color(COLOR_RESET) << endl;
                cout << get_color(COLOR_RED) + "connect [host] [port]    [name]" + get_color(COLOR_RESET) << endl;
            }
            else if(command[3].size() < 5){
                cout << get_color(COLOR_RED) + "name is should be long than 4 words" + get_color(COLOR_RESET) << endl;
            }
            else if(is_login){
                cout << get_color(COLOR_RED) + "you had logged in" + get_color(COLOR_RESET) << endl;
            }
            else if(cout << "connect..."  << endl && doConnect(command[1], command[2]) ){
                cout << "connect success" << endl;
                cout << "login..." << endl;
                int n = doLogin(command[3]);
                //cout << "n = " << n << endl;
                if(n == FAILED){
                    cout << get_color(COLOR_RED) + "some thing failed" + get_color(COLOR_RESET) << endl;
                    close(client_socket);
                }
                else if(n == FAILED_NAME){
                    cout << get_color(COLOR_RED) + "invalid name" + get_color(COLOR_RESET) << endl;
                    close(client_socket);
                }
                else if(n == IS_ONLINE){
                    cout << get_color(COLOR_RED) + "this name had logged in" + get_color(COLOR_RESET) << endl;
                    close(client_socket);
                }
                else{
                    is_login = true;
                    cout << "login success" << endl;
                }
            }
            else{
                cout << get_color(COLOR_RED) + "connect failed"+get_color(COLOR_RESET) << endl;
            }
        }
        else if(command[0] == "chat"){
            if(command.size() < 3 ){
                cout << get_color(COLOR_RED) + "error command" + get_color(COLOR_RESET) << endl;
                cout << get_color(COLOR_RED) + "chat    [name] [name]... [\"message\"]" + get_color(COLOR_RESET) << endl;
            }
            else if(!is_login){
                cout << get_color(COLOR_RED) + "in disconnect state, please connect" + get_color(COLOR_RESET) << endl;
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
                        cout << get_color(COLOR_RED) + "failed to transmit, disconnect, please connect again" + get_color(COLOR_RESET) << endl;
                        break;
                    }
                    else if(n == USER_NOT_EXIST) cout << get_color(COLOR_RED) + command[i] << " not a member of this chatroom" + get_color(COLOR_RESET) << endl;
                    else if(n == USER_OFFLINE) cout << get_color(COLOR_B_BLUE) + command[i] << " is offline, he/she will see this message whe he/she login" + get_color(COLOR_RESET) << endl;
                }
            }
        }
        else if(command[0] == "bye" || command[0] == "exit"){
            if(command[0] == "bye" && !is_login){
                cout << get_color(COLOR_RED) + "in disconnect state, please connect" + get_color(COLOR_RESET) << endl;
            }
            else if(is_login){
                int w = write(client_socket, "bye", socket_buffer_size);
                int n;
                n = read_server_flag();
                is_login = false;
                close(client_socket);
                cout << "disconnect" << endl;
            }
            if(command[0] == "exit") break;
        }
        else if(command[0] == "color"){
            int color;
            if(command.size() < 2 || ((color = string_to_int(command[1])) < 2 || color > 11)){
                cout << get_color(COLOR_RED) + "error command" + get_color(COLOR_RESET) << endl;
                cout << get_color(COLOR_RED) + "color [number]" + get_color(COLOR_RESET) << endl;
                show_color_choice();
            }
            else if(!is_login){
                cout << get_color(COLOR_RED) + "in disconnect state, please connect" + get_color(COLOR_RESET) << endl;
            }
            else{
                int w = write(client_socket, (command[0]+":"+command[1]).c_str(), socket_buffer_size);
            }
        }
        else{
            cout << get_color(COLOR_RED) + "invalid command" + get_color(COLOR_RESET) << endl;
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
    cout << "color [number]" << endl;
    show_color_choice();
    cout << "bye" << endl;
    cout << "exit" << endl;
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


static void show_color_choice(){

    for(int i = 2 ; i <= 11 ; i++){
        cout << get_color(i) + "display " << i << " color" << get_color(COLOR_RESET) << endl;
    }

}
