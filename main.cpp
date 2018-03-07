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
        auto c = new Async_Connect(&loop, "192.99.161.69", 80, 10);
        c->connected_event = [c, &loop](int d){
            cout << "Async_Connect connected" << endl;
            delete c;

            string http_request = "GET / HTTP/1.1\r\nHost: masterchan.me\r\nConnection: close\r\nUser-Agent: Paw/3.1.4 (Macintosh; OS X/10.13.3) GCDHTTPRequest\r\n\r\n";
            char empty_space[200];

            // Async_Write Example
            char * write_buffer = strdup(http_request.c_str());
            ssize_t write_size = http_request.length();
            auto aw = new Async_Write(&loop, d, write_buffer, write_size);
            aw->wrote_event = [](){
                cout << "Async_Write sent" << endl;
            };
            aw->closed_event = [](){};
            aw->failed_event = [](){};
            aw->start();


            // Async_Read Example
            char * read_buffer = empty_space;
            ssize_t read_size = 200;
            auto ar = new Async_Read(&loop, d, read_buffer, read_size);
            ar->set_timeout(10);
            ar->read_event = [read_buffer](){
                cout << "Async_Read read: "<< string(read_buffer) << endl;
            };
            ar->closed_event = [](){
                cout << "Async_Read closed" << endl;
            };
            ar->failed_event = [](){
                cout << "Async_Read failed" << endl;
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