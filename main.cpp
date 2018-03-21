#include "main.hpp"

using namespace std;


int main(int argc, char **argv) {
    //Signal issue
    signal(SIGPIPE, SIG_IGN);

    //EV loop
    ev::default_loop loop;

    //EV listener
    Timeout_Listener timeout_l(&loop, 0, 1);
    timeout_l.after_launch = [argc, argv](Container *c) {
        if (argc > 1) {
            string s(argv[1]);
            if (s == "client") {
                c->mode = "client         ";
                c->cc = new Client_Core(c->loop);
                c->cc->start();

            } else if (s == "peer") {
                c->mode = "peer           ";
                c->pc = new Peer_Core(c->loop);
                c->pc->start();

            } else if (s == "socks5") {
                c->mode = "server-socks5  ";
                c->sc = new Socks_Core(c->loop);
                c->sc->start();
            }

        } else {
            c->mode = "test           ";
        }

        for (int i = 1; i < argc; ++i) {
            string s(argv[i]);
            if (s == "clear"){
                c->clear_screen = true;
            }
        }
    };
    timeout_l.repeat_event = [](Container *c, int i) {

        // DEBUG
        if (c->clear_screen){
            printf("\033c");
        }
        cout << c->mode << " - Page " << i << "\tTime " << c->get_time() << endl;
        if (c->cc != nullptr) {
            cout << "\n-----------------------------------" << endl;
            cout << "Client A: Browser to Client Program" << endl;
            for (const auto &kv : c->cc->connection_a) {
                if (kv.second == nullptr)continue;
                cout << kv.first << "\t" << kv.second->info() << endl;
            }
            cout << "\n-----------------------------------" << endl;
            cout << "Client B: Client Program to Proxy Peer" << endl;
            for (const auto &v : c->cc->connection_b) {
                cout << v->info() << endl;
            }
            cout << "\n-----------------------------------" << endl;
            cout << "Client B: Client Program to Proxy Peer - Active Connection" << endl;
            for (const auto &v : c->cc->connection_b_available) {
                cout << v->info() << endl;
            }
            cout << "\n-----------------------------------" << endl;
        }
        if (c->pc != nullptr) {
            cout << "\n-----------------------------------" << endl;
            cout << "Peer A: Client Program to Proxy Peer" << endl;
            for (const auto &v : c->pc->connection_a) {
                cout << v->info() << endl;
            }
            cout << "\n-----------------------------------" << endl;
            cout << "Peer A: Client Program to Proxy Peer - Active Connection" << endl;
            for (const auto &v : c->pc->connection_a_available) {
                cout << v->info() << endl;
            }
            cout << "\n-----------------------------------" << endl;
            cout << "Peer B: Proxy Peer to SOCKS proxy server" << endl;
            for (const auto &kv : c->pc->connection_b) {
                if (kv.second == nullptr)continue;
                cout << kv.first << "\t" << kv.second->info() << endl;
            }
            cout << "\n-----------------------------------" << endl;

        }
    };

    Command_Listener command_l(&loop);

    //Start
    loop.run();

    return 0;
}