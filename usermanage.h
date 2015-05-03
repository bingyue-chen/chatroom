#include "user.h"
#include <vector>

using namespace std;

class Usermanage {

public:
    Usermanage();

    /* [0 => new user, 1 => old user, -1 => is online, -2 => failed name, -3 => failed] */
    int add_user(User);

    void user_exit(string user_ip);

    /* [0 => send success, 1 => user offline, 2 => user not exit] */
    int send_msg(string from_user_ip, string message, string to_user_ip);

    void broadcast(string message);

    int get_active_user_amount();
    void show();

private:
    vector<User> users;
    int active_user_amount;

    int find_user(string key, string value);
    bool is_same_name(string name);
};
