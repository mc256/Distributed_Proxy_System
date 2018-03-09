#include "main.hpp"

using namespace std;


int main(int argc, char **argv) {
    //Signal issue
    signal(SIGPIPE, SIG_IGN);

    //EV loop
    ev::default_loop loop;

    //EV listener
    Timeout_Listener timeout_l(&loop, 0, 2);
    timeout_l.after_launch = [argc, argv] (Container * c){
        if (argc > 1){
            string s(argv[1]);
            if (s == "client") {
                c->mode = "client         ";
                c->cc = new Client_Core(c->loop);
                c->cc->start();

            }else if (s == "peer-server") {
                c->mode = "peer           ";



            }else if (s == "server-socks5") {
                c->mode = "server-socks5  ";



            }
        }else{
            c->mode = "test           ";



        }
    };
    timeout_l.repeat_event = [] (Container * c, int i){
        cout << c->mode << "---------------------->" << i << endl;

    };

    Command_Listener command_l(&loop);

    //Start
    loop.run();

    return 0;
}