#include "user.h"
#include <vector>

using namespace std;

class Usermanage {

public:
    Usermanage();

    int add_user(User);
    string user_exit(int fd);
    int send_msg(int from, string message, string to_user_name);
    void broadcast(string message);
    int get_active_user_amount();
    void show();
    void push_offline_line(int fd);
    void setcolor(int fd, int color);

private:
    vector<User> users;
    int active_user_amount;

    int find_user(string key, string value);
    bool is_same_name(string name);
};
