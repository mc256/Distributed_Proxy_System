//
// Created by mc on 2/23/18.
//

#include "Handshake.hpp"


void Handshake::start() {
    //Check SOCKS protocol

    //c2p version
    struct SOCKS_Version_Client c2p_version;
    CHECK(recv(p2c_descriptor, &c2p_version, sizeof(struct SOCKS_Version_Client), 0), <= 0,
          p2c_descriptor << "\tCannot establish SOCKS connection",);
    CHECK(c2p_version.ver, != 0x05, p2c_descriptor << "\tNot supported SOCKS version.",);

    bool has_support_method = false;
    for (int i = 0; i < c2p_version.nmethods; ++i) {
        if (c2p_version.methods[i] == 0x00) {
            has_support_method = true;
            break;
        }
    }
    CHECK(!has_support_method, , p2c_descriptor << "\tNo supported SOCKS methods.",);

    //p2c version
    struct SOCKS_Version_Server p2c_version;
    p2c_version.ver = 0x05;
    p2c_version.method = 0x00;
    CHECK(send(p2c_descriptor, &p2c_version, sizeof(struct SOCKS_Version_Server), 0), <= 0,
          p2c_descriptor << "\tCannot send version information.",);

    //c2p request
    struct SOCKS_Request_Client c2p_request;
    struct SOCKS_Request_Server p2c_request;
    size_t p2c_request_length = 4;
    CHECK(recv(p2c_descriptor, &c2p_request, sizeof(struct SOCKS_Request_Client), 0), <= 0,
          p2c_descriptor << "\tCannot read request information.",);

    DEBUG(cout << p2c_descriptor << "\tVersion:" << +c2p_request.ver << " CMD:" << +c2p_request.cmd << " ATYP:"
               << +c2p_request.atyp << endl;);
    CHECK(c2p_request.cmd, !=0x01, p2c_descriptor << "\tNot supported command.",);

    memset(&p2c_request, 0, sizeof(struct SOCKS_Request_Server));
    p2c_request.ver = 0x05;
    p2c_request.rep = 0x00;
    p2c_request.rsv = 0x00;

    struct sockaddr_storage request_dst_address;
    socklen_t request_dst_address_length = sizeof(struct sockaddr_storage);
    if (c2p_request.atyp == 0x01) {
        //IPv4
        struct sockaddr_in *ipv4 = (struct sockaddr_in *) &request_dst_address;
        ipv4->sin_family = AF_INET;
        memcpy(&(ipv4->sin_port), &(c2p_request.dst[4]), 2);
        memcpy(&(ipv4->sin_addr), &(c2p_request.dst[0]), 4);
        request_dst_address_length = sizeof(struct sockaddr_in);

    } else if (c2p_request.atyp == 0x04) {
        //IPv6
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) &request_dst_address;
        ipv6->sin6_family = AF_INET6;
        memcpy(&(ipv6->sin6_port), &(c2p_request.dst[16]), 2);
        memcpy(&(ipv6->sin6_addr), &(c2p_request.dst[0]), 16);
        request_dst_address_length = sizeof(struct sockaddr_in6);

    } else if (c2p_request.atyp == 0x03) {
        //Domain
        char domain_buffer[256];
        memcpy(&domain_buffer, &(c2p_request.dst[1]), c2p_request.dst[0]);
        domain_buffer[c2p_request.dst[0]] = '\0';

        string port_buffer = to_string(+ntohs(*((uint16_t *) &(c2p_request.dst[1 + c2p_request.dst[0]]))));
        struct addrinfo *resolve;
        if (getaddrinfo(domain_buffer, port_buffer.c_str(), NULL, &resolve)) {
            DEBUG(cout << p2c_descriptor << "\tgetaddrinfo() ERROR" << endl;);
            p2c_request.rep = 0x04;
        } else {
            struct addrinfo *p = resolve;
            p2c_request.rep = 0x04;
            while (p != NULL) {
                if (p->ai_socktype == SOCK_STREAM) {
                    p2c_request.rep = 0x00;
                    memcpy(&request_dst_address, p->ai_addr, p->ai_addrlen);
                    request_dst_address_length = p->ai_addrlen;
                    break;
                }
                p = p->ai_next;
            }
            freeaddrinfo(resolve);
        }
    }

    //Connect to Server
    // If anything goes wrong, let the client know.
    int p2s_descriptor;
    CHECK_DO(p2s_descriptor = socket(((sockaddr *) &request_dst_address)->sa_family, SOCK_STREAM, 0), < 0,
             p2c_descriptor << "\tFail to create socket!", p2c_request.rep = 0x01);
    CHECK_DO(connect(p2s_descriptor, (sockaddr *) &request_dst_address, request_dst_address_length), ,
             p2c_descriptor << "\tSocket connection failed!", p2c_request.rep = 0x01);

    struct sockaddr_storage request_bound_storage;
    struct sockaddr *request_bound = (sockaddr *) &request_bound_storage;
    socklen_t request_bound_length = sizeof(sockaddr_storage);


    if (p2c_request.rep == 0x00) {
        p2c_request.rep = 0x08;
        if (getsockname(p2s_descriptor, request_bound, &request_bound_length) == 0) {
            //address information
            if (request_bound->sa_family == AF_INET) {
                p2c_request.rep = 0x00;
                p2c_request.atyp = 0x01;
                memcpy(&(p2c_request.bnd[4]), &(((sockaddr_in *) request_bound)->sin_port), 2);
                memcpy(&(p2c_request.bnd), &(((sockaddr_in *) request_bound)->sin_addr), 4);
                p2c_request_length = 4 + 4 + 2;
            } else if (request_bound->sa_family == AF_INET6) {
                p2c_request.rep = 0x00;
                p2c_request.atyp = 0x04;
                memcpy(&(p2c_request.bnd[16]), &(((sockaddr_in6 *) request_bound)->sin6_port), 2);
                memcpy(&(p2c_request.bnd), &(((sockaddr_in6 *) request_bound)->sin6_addr), 16);
                p2c_request_length = 4 + 16 + 2;
            }
        }
    }

    CHECK_DO(send(p2c_descriptor, &p2c_request, p2c_request_length, 0), <=0, p2c_descriptor << "\tClient Lost",
             close(p2c_descriptor);
                     close(p2s_descriptor);
                     return);
    CHECK_DO(p2c_request.rep, != 0x00, p2c_descriptor << "\tClosed", close(p2c_descriptor);
            return);

    fcntl(p2s_descriptor, F_SETFL, fcntl(p2s_descriptor, F_GETFL) | O_NONBLOCK);
    fcntl(p2c_descriptor, F_SETFL, fcntl(p2c_descriptor, F_GETFL) | O_NONBLOCK);

    Channel *c2s = new Channel(this->loop, p2c_descriptor, p2s_descriptor);
    Channel *s2c = new Channel(this->loop, p2s_descriptor, p2c_descriptor);
    c2s->setPair(s2c);
    s2c->setPair(c2s);
    c2s->start();
    s2c->start();
    delete this;
}

Handshake::Handshake(ev::default_loop *loop, int p2c_descriptor) {
    this->loop = loop;
    this->p2c_descriptor = p2c_descriptor;
}