#include "user.h"
#include <vector>

using namespace std;

class Usermanage {

public:
    Usermanage();

    int add_user(User);
    void user_exit(string user_ip);
    int send_msg(string from_user_ip, string message, string to_user_ip);
    void broadcast(string message);
    int get_active_user_amount();
    void show();
    void push_offline_line(string ip);
    void setcolor(string ip, int color);

private:
    vector<User> users;
    int active_user_amount;

    int find_user(string key, string value);
    bool is_same_name(string name);
};
