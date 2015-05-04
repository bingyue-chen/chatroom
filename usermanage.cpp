#include "usermanage.h"

Usermanage::Usermanage(){
     this->active_user_amount = 0;
}

int Usermanage::add_user(User user){
    int found = this->find_user("ip", user.get("ip"));

    if(found == -1){
        if(this->is_same_name(user.get("name"))) return FAILED_NAME;
        this->users.push_back(user);
        this->active_user_amount++;
        return NEW_USER;
    }
    else{
        if(this->users[found].is_online()) return IS_ONLINE;
        if((user.get("name") != this->users[found].get("name")) && this->is_same_name(user.get("name"))) return FAILED_NAME;
        this->users[found].online(string_to_int(user.get("fd")), string_to_int(user.get("port")), user.get("name"));
        this->active_user_amount++;
        return OLD_USER;
    }

    return FAILED;
}

void Usermanage::user_exit(string user_ip){
    int found = this->find_user("ip", user_ip);

    if(found != -1){
        this->users[found].offline();
        this->active_user_amount--;
    }
}

bool Usermanage::is_same_name(string name){
    return (find_user("name", name) != -1);
}

int Usermanage::find_user(string key, string value){
    unsigned int users_size = this->users.size();
    unsigned int i;

    for(i = 0; i < users_size; i++)
        if (this->users[i].get(key) == value)
            return i;

    return -1;
 }

int Usermanage::send_msg(string from_user_ip, string message, string to_user_name){
    int from = this->find_user("ip", from_user_ip);
    int to   = this->find_user("name", to_user_name);

    if(from == -1 || to == -1) return USER_NOT_EXIST;

    message = this->users[from].get("name")+ " : " + message + "\n";
    int from_color = string_to_int(this->users[from].get("color"));
    if( from_color > 2)
        message = get_color(from_color) + message + get_color(COLOR_RESET);

    if(this->users[to].send(message)) return SEND_SUCCESS;
    else return USER_OFFLINE;
}

void Usermanage::broadcast(string message){
    message = get_color(COLOR_B_BLUE) + message + get_color(COLOR_RESET);
    for(unsigned int i = 0; i < this->users.size(); i++){
        if(this->users[i].is_online())
            this->users[i].send(message);
    }
}

int Usermanage::get_active_user_amount(){
    return this->active_user_amount;
}

void Usermanage::show(){
    if(this->users.size() > 0)
        printf("%20s%15s%15s%15s\n","Username", "Ip", "Port", "on/off line");
    for(unsigned int i = 0; i < this->users.size(); i++){
        printf("%20s%15s%15s%15s\n",
               this->users[i].get("name").c_str(),
               this->users[i].get("ip").c_str(),
               this->users[i].get("port").c_str(),
               (this->users[i].is_online()) ? "on" : "off"
        );
    }

    printf("%-5donline\n", this->active_user_amount);
}

void Usermanage::push_offline_line(string ip){
    int found = this->find_user("ip", ip);

    if(found != -1 && this->users[found].is_online())
        this->users[found].send_offline_message();
}

void Usermanage::setcolor(string ip, int color){
    int found = this->find_user("ip", ip);

    if(found != -1)
        this->users[found].setcolor(color);
}

