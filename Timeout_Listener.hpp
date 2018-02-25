//
// Created by mc on 2/23/18.
//


#ifndef PRPR_TIMEOUT_LISTENER_HPP
#define PRPR_TIMEOUT_LISTENER_HPP

#include "main.hpp"

class Timeout_Listener {

    private:
        ev::timer watcher;
        ev::default_loop * loop;
        int count;
        bool first = true;

        int argc;
        char ** argv;

        Server_Accept<Handshake> * sa_h;
        Server_Accept<Client_A> * sa_cau;
        Server_Accept<Peer_A> * sa_pau;
        Server_Accept<Peer_A> * sa_pau1;
        Server_Accept<Peer_A> * sa_pau2;
        Server_Accept<Peer_A> * sa_pau3;
        Server_Accept<Peer_A> * sa_pau4;
        Server_Connect<Client_B> * sc_test;
        struct Proxy_Peer * pp;

        void launch_program();
        void refresh_console();
        void callback(ev::timer &watcher, int revents);


        string program_mode = "";
    public:
        // Constructor
        Timeout_Listener(ev::default_loop * loop, double after, double repeat, int argc, char ** argv);

};



#endif //PRPR_TIMEOUT_LISTENER_HPP
