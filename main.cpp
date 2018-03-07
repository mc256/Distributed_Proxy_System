#include "main.hpp"

typedef function<void()> function1;
using namespace std;


int main (int argc, char ** argv) {
    //Signal issue
    signal(SIGPIPE, SIG_IGN);

    //EV loop
    ev::default_loop loop;
    function1 after_launch = [&argc, &argv, &loop]{









        };
    function<void(int)> repeat_event = [](int d){

    };

    //EV listener
    Timeout_Listener timeout_l(&loop, 0, 2, &after_launch, &repeat_event);
    Command_Listener command_l(&loop);

    //Start
    loop.run();

    return 0;
}