#include "main.hpp"

using namespace std;


int main(int argc, char **argv) {
    //Signal issue
    signal(SIGPIPE, SIG_IGN);

    //EV loop
    ev::default_loop loop;


    function<void()> after_launch = [&argc, &argv, &loop] {

        /////////////////////////////////////////
        // Async_Connect Example
        auto c = new Async_Connect(&loop, "127.0.0.1", 25, 10);
        c->connected_event = [c, &loop](int d){
            cout << "Async_Connect connected" << endl;
            delete c;




            char buf[10];
            auto ar = new Async_Read(&loop, d, buf, 10);
            ar->set_timeout(10);
            ar->read_event = [buf](){
                cout << string(buf) << endl;
            };
            ar->closed_event = [](){
                cout << "test" << endl;
            };
            ar->failed_event = [](){
                cout << "test222" << endl;
            };
            ar->start();




        };
        c->failed_event = [c](){
            cout << "Async_Connect timeout" << endl;
            delete c;
        };
        c->start();

        // Async_Accept Example
        auto  a = new Async_Accept(&loop, "0.0.0.0", 9100);
        a->accepted_event = [a](int d, sockaddr_storage s, socklen_t sl){
            cout << "Async_Accept accepted" << endl;
        };
        a->start();

        // Async_Read Example


        /////////////////////////////////////////
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