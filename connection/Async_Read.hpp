//
// Created by mc on 3/7/18.
//

#ifndef PRPR_ASYNC_READ_HPP
#define PRPR_ASYNC_READ_HPP

#include "../main.hpp"

class Async_Read {
private:
    int descriptor;

    char * buffer;
    ssize_t position;
    ssize_t length;

    int timeout;
    bool undefined_length;
    bool use_recv;

    ev::io read_io_watcher;
    ev::timer timeout_watcher;

public:
    // Stops all the event listener
    void stop_watchers();

    // Event call back function
    void read_callback(ev::io &w, int r);
    void timeout_callback(ev::timer &w, int r);

    // The function will be called when the buffer has been filled up
    function <void(char *, ssize_t)> read_event;
    // The function will be called when it read anything
    function <void(char *, ssize_t)> recv_event = nullptr;
    // The function will be called when the connection is closed
    function <void(char *, ssize_t)> closed_event;
    // The function will be called when it is timeout or failed
    function <void(char *, ssize_t)> failed_event;

    // Set the timeout for the connection
    //  i: number of seconds, if it is ZERO, then no timeout
    void set_timeout(int i);
    // Set the timeout for the connection
    //  b: if true then read whatever it can read and return the object through read_event immediately.
    void set_undefined_length(bool b);
    // Set if the user want to use recv_event
    //  b: if true then the user wants to use recv_event instead of read_event
    void set_use_recv(bool b);

    // Starts reading data from the socket
    void start();
    // Resets the read pointer of the buffer and the timeout
    void reset();
    // Resets the buffer by assigning a new buffer and a new length
    //  buffer: pointer to a char buffer
    //  length: the expected length of the data that can read from the socket
    void reset(char *buffer, ssize_t length);

    // Constructor
    //  Just for initialize, we do not know if the reader need to write anything to the buffer or not/
    //  We can define it later.
    //  loop: event loop
    //  descriptor: is the socket descriptor
    Async_Read(ev::default_loop *loop, int descriptor);

    // Constructor
    //  Creates a asynchronous socket reader
    //  loop: event loop
    //  descriptor: is the socket descriptor
    //  buffer: is the buffer that the reader store the data
    //  length: expected length of the data
    Async_Read(ev::default_loop *loop, int descriptor, char * buffer, ssize_t length);

    // Destructor
    ~Async_Read();

};


#endif //PRPR_ASYNC_READ_HPP
