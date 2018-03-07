//
// Created by mc on 2/23/18.
//

#ifndef PRPR_MAIN_H
#define PRPR_MAIN_H


#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <list>
#include <functional>
#include <algorithm>


#include <sys/socket.h> //socket
#include <sys/un.h>     //sockaddr_un
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>     //close()
#include <csignal>
#include <fcntl.h>

#include <netdb.h>
#include <ev++.h>
#include <cstdlib>

#include <atomic>
#include <thread>
#include <time.h>

#include <openssl/sha.h>

#define UCHAR unsigned char
#define CHECK(a, b, c, d)  if ( (a) b ){ cout << c << endl; return d;}
#define CHECK_DO(a, b, c, d)  if ( (a) b ){ cout << c << endl; d;}
#define DEBUG(a) if (true) {a}

#define MAX_BUFFER_SIZE 2048
#define RESEND_PERIOD 30

struct SOCKS_Version_Client{
    UCHAR ver;
    UCHAR nmethods;
    UCHAR methods[255];
};

struct SOCKS_Version_Server{
    UCHAR ver;
    UCHAR method;
};

struct SOCKS_Request_Client{
    UCHAR ver;
    UCHAR cmd;
    UCHAR rsv;
    UCHAR atyp;
    UCHAR dst[1 + 255 + 2]; //use SOCKS_Address_
};

struct SOCKS_Request_Server{
    UCHAR ver;
    UCHAR rep;
    UCHAR rsv;
    UCHAR atyp;
    UCHAR bnd[1 + 255 + 2]; //use SOCKS_Address_
};

struct Data_Package{ // Data structure designed for transmission
    int size;
    int sent;
    int dispatcher_id;
    UCHAR buffer[MAX_BUFFER_SIZE];
    time_t timestamp;
};


struct Data_Meta{
    UCHAR dispatcher_id[4]; //int
    UCHAR user_id[4]; // int
    UCHAR sequence_id[4]; // int
    UCHAR size[4]; //int
};

struct Proxy_Peer{
    std::string address;
    int port;
    int descriptor;
    bool connected;
    bool available;
    std::string password;
    void * interface;
};

using namespace std;


class Encryption;
class Channel;
class Handshake;
class W_Filter;
class R_Filter;
class Client_A;
class Client_B;
class Peer_A;
class Peer_B;
template <class T> class Server_Connect;
template <class T> class Server_Accept;
class Client_Core;
class Timeout_Listener;
class Command_Listener;


#include "Encryption.hpp"
#include "Channel.hpp"
#include "Handshake.hpp"
#include "W_Filter.hpp"
#include "R_Filter.hpp"
#include "Client_A.hpp"
#include "Client_B.hpp"
#include "Peer_A.hpp"
#include "Peer_B.hpp"
#include "Server_Accept.hpp"
#include "Server_Connect.hpp"
#include "Client_Core.hpp"
#include "Peer_Core.hpp"
#include "Z_Test_Connect.hpp"
#include "connection/Async_Connect.hpp"
#include "connection/Async_Accept.hpp"
#include "connection/Async_Read.hpp"
#include "utility/Timeout_Listener.hpp"
#include "utility/Command_Listener.hpp"



#endif //PRPR_MAIN_H
