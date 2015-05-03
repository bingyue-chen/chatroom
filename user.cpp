#include "user.h"


User::User(int fd, string name, string ip, int port){
    this->fd = fd;
    this->name = name;
    this->ip = ip;
    this->port = port;
    this->active = true;
}

bool User::send(string message){
    int write_size;
    if(this->is_online()){
        write_size = write(this->fd, message.c_str(), socket_buffer_size);
        if(write_size > 0){
            //fsync(this->fd);
            return true;
        }
        else
            this->offline();
    }

    if(!this->is_online()) {
        message = message + " sent at " + this->get_now_time() + "\n";
        this->offline_messager.push_back(message);
    }

    return false;
}

void User::offline(){
    this->active = false;
}

void User::online(int fd, int port, string name){

    this->active = true;
    this->fd = fd;
    this->port = port;
    this->name = name;

    while(!this->offline_messager.empty()){
        this->send(this->offline_messager.back());
        this->offline_messager.pop_back();
    }
}

bool User::is_online(){
    return this->active;
}

string User::get(string key){
   if(key == "name") return this->name;
   else if(key == "ip") return this->ip;
   else if(key == "fd") return int_to_string(this->fd);
   else if(key == "port") return int_to_string(this->port);
   else return "";
}

string User::get_now_time(){

    time_t currentTime;
    struct tm *localTime;

    time( &currentTime );                   // Get the current time
    localTime = localtime( &currentTime );  // Convert the current time to the local time

    int Day    = localTime->tm_mday;
    int Month  = localTime->tm_mon + 1;
    int Year   = localTime->tm_year + 1900;
    int Hour   = localTime->tm_hour;
    int Min    = localTime->tm_min;
    int Sec    = localTime->tm_sec;

    string s =  int_to_string(Year) + "//" + int_to_string(Month) + "//" + int_to_string(Day);
           s += " " + int_to_string(Hour) + ":" + int_to_string(Min) + ":" + int_to_string(Sec);

    return s;
}
