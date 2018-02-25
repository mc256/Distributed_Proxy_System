#include "main.hpp"

using namespace std;


int main (int argc, char ** argv) {

    //Signal issue
    signal(SIGPIPE, SIG_IGN);

    //EV loop
    ev::default_loop loop;
    Timeout_Listener timeout_l(&loop, 0, 2, argc, argv);
    Command_Listener command_l(&loop);
    loop.run();

    return 0;
}