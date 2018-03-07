#include "main.hpp"

using namespace std;


int main(int argc, char **argv) {
    //Signal issue
    signal(SIGPIPE, SIG_IGN);

    //EV loop
    ev::default_loop loop;


    function<void()> after_launch = [&argc, &argv, &loop] {

        auto c = new Async_Connect(&loop, "158.69.34.50", 80, 10);
        c->connected_event = [](int d){
            cout << "connected22" << endl;
        };
        c->failed_event = [](){
            cout << "timeout" << endl;
        };
        c->start();


    };
    function<void(int)> repeat_event = [](int d) {

    };

    //EV listener
    Timeout_Listener timeout_l(&loop, 0, 2, &after_launch, &repeat_event);
    Command_Listener command_l(&loop);

    //Start
    loop.run();

    return 0;
}