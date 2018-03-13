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
#define DEBUG(a) if (false) {a}

#define MAX_BUFFER_SIZE 2048
#define FAKE_HEADER_SIZE 600
#define READ_BUFFER_SIZE 100
#define RESEND_PERIOD 5
#define ACK_CHANCE 50
#define DEFAULT_TIMEOUT 30

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


struct Packet_Meta{
    int dispatcher;
    int signal;

    size_t sequence;
    size_t size;
};




using namespace std;

class Container;
class Encryption;
class Packet;
class Proxy_Peer;

class Timeout_Listener;
class Command_Listener;

class Async_Connect;
class Async_Accept;
class Async_Read;
class Async_Write;

class Client_A;
class Client_B;

class Peer_A;
class Peer_B;

class Client_Core;
class Peer_Core;
class Socks_Core;

class Channel;
class Handshake;

#include "utility/Container.hpp"
#include "utility/Encryption.hpp"
#include "utility/Packet.hpp"
#include "utility/Proxy_Peer.hpp"

#include "connection/Async_Connect.hpp"
#include "connection/Async_Accept.hpp"
#include "connection/Async_Read.hpp"
#include "connection/Async_Write.hpp"

#include "utility/Timeout_Listener.hpp"
#include "utility/Command_Listener.hpp"

#include "interface/Client_A.hpp"
#include "interface/Client_B.hpp"

#include "interface/Peer_A.hpp"
#include "interface/Peer_B.hpp"

#include "program/Client_Core.hpp"
#include "program/Peer_Core.hpp"
#include "program/Socks_Core.hpp"

#include "socks5/Channel.hpp"
#include "socks5/Handshake.hpp"


#endif //PRPR_MAIN_H
