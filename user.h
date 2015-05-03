#include <string>
#include <unistd.h>
#include <vector>
#include <ctime>
#include "global_def.h"

using namespace std;

class User {

public:
    User(int fd, string name, string ip, int port);
    bool send(string message);
    void offline();
    void online(int fd, int port, string name);
    bool is_online();
    string get(string key);

private:
    int fd, port;
    string name, ip;
    bool active;
    vector<string> offline_messager;
    string get_now_time();
};
