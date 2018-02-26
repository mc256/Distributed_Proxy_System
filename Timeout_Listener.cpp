//
// Created by mc on 2/23/18.
//

#include "Timeout_Listener.hpp"


void Timeout_Listener::launch_program() {
    //Server_Connect<int> * test = new Server_Connect<int>(loop, "192.99.161.69", 80);
    if (this->argc > 1) {
        istringstream iss;
        iss.str(this->argv[1]);
        if (iss.str().compare("server-socks5") == 0) {
            program_mode = "server-socks5  ";
            sa_h = new Server_Accept<Handshake>(loop, "0.0.0.0", 9099);



        } else if (iss.str().compare("client") == 0) {
            program_mode = "client         ";
            (new Client_Core(loop, "0.0.0.0", 9100));




        } else if (iss.str().compare("peer-server") == 0) {
            program_mode = "peer           ";
            sa_pau1 = new Server_Accept<Peer_A>(loop, "0.0.0.0", 9101);
            sa_pau2 = new Server_Accept<Peer_A>(loop, "0.0.0.0", 9102);
            sa_pau3 = new Server_Accept<Peer_A>(loop, "0.0.0.0", 9103);
            sa_pau4 = new Server_Accept<Peer_A>(loop, "0.0.0.0", 9104);


        }
    } else {
        // what we are testing
        /*
        program_mode = "testing        ";
        pp = new struct Proxy_Peer;
        pp->address = "192.99.161.69";
        pp->port = 9100;
        pp->password = "iampassword";
        sc_test = new Server_Connect<Client_B>(loop, pp);
        */
        //Client_B::generate_fake_request(pp);
        program_mode = "client         ";
        (new Client_Core(loop, "0.0.0.0", 9100));
    }
}

void Timeout_Listener::refresh_console() {
    count++;
    //printf("\033c");
    cout << program_mode << "---------------------->" << count << endl;
    for (const auto &kv : Client_A::interface_list) {
        std::cout << kv.first << "\t=> " << ((Client_A *) kv.second)->info() << std::endl;
    }
    for (const auto &kv : Client_B::interface_list) {
        std::cout << kv.first << "\t=> " << ((Client_B *) kv.second)->info() << std::endl;
    }
    for (const auto &kv : Peer_A::interface_list) {
        std::cout << kv.first << "\t=> " << ((Peer_A *) kv.second)->info() << std::endl;
    }
}

void Timeout_Listener::callback(ev::timer &watcher, int revents) {
    if (first) {
        first = false;
        this->launch_program();
    } else {
        this->refresh_console();
    }
}


// Constructor
Timeout_Listener::Timeout_Listener(ev::default_loop *loop, double after, double repeat, int argc, char **argv) {
    this->count = 0;
    this->watcher.set(*loop);
    this->loop = loop;
    this->argc = argc;
    this->argv = argv;
    this->watcher.set<Timeout_Listener, &Timeout_Listener::callback>(this);
    this->watcher.start(after, repeat);
}
