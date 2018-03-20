#include "main.hpp"

using namespace std;


int main(int argc, char **argv) {
    //Signal issue
    signal(SIGPIPE, SIG_IGN);

    //EV loop
    ev::default_loop loop;

    //EV listener
    Timeout_Listener timeout_l(&loop, 0, 2);
    timeout_l.after_launch = [argc, argv](Container *c) {
        if (argc > 1) {
            string s(argv[1]);
            if (s == "client") {
                c->mode = "client         ";
                c->cc = new Client_Core(c->loop);
                c->cc->start();

            } else if (s == "peer-server") {
                c->mode = "peer           ";
                c->pc = new Peer_Core(c->loop);
                c->pc->start();

            } else if (s == "server-socks5") {
                c->mode = "server-socks5  ";
                c->sc = new Socks_Core(c->loop);
                c->sc->start();

            }
        } else {
            c->mode = "test           ";


        }
    };
    timeout_l.repeat_event = [](Container *c, int i) {
        // JOBS
        if (c->cc != nullptr){
            c->cc->schedule_check();
        }


        // DEBUG
        printf("\033c");
        cout << c->mode << "---------------------->" << i << endl;
        if (c->cc != nullptr) {
            cout << "Client A:" << endl;
            for (const auto &kv : c->cc->connection_a) {
                if (kv.second == nullptr)continue;
                cout << kv.first << "\t" << kv.second->info() << endl;
            }
            cout << "Client B:" << endl;
            for (const auto &v : c->cc->connection_b) {
                cout << v->info() << endl;
            }
            cout << "Client B available:" << endl;
            for (const auto &v : c->cc->connection_b_available) {
                cout << v->info() << endl;
            }
        }
        if (c->pc != nullptr) {
            cout << "Peer A:" << endl;
            for (const auto &v : c->pc->connection_a) {
                cout << v->info() << endl;
            }
            cout << "Peer A available:" << endl;
            for (const auto &v : c->pc->connection_a_available) {
                cout << v->info() << endl;
            }
            cout << "Peer B:" << endl;
            for (const auto &kv : c->pc->connection_b) {
                if (kv.second == nullptr)continue;
                cout << kv.first << "\t" << kv.second->info() << endl;
            }

        }
    };

    Command_Listener command_l(&loop);

    //Start
    loop.run();

    return 0;
}