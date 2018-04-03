//
// Created by mc on 3/7/18.
//

#ifndef PRPR_ASYNC_WRITE_HPP
#define PRPR_ASYNC_WRITE_HPP

#include "../main.hpp"

class Async_Write {
private:
    int descriptor;

    char * buffer;
    ssize_t position;
    ssize_t length;

    int timeout;

    ev::io write_io_watcher;
    ev::timer timeout_watcher;

public:
    // Stops all the event listener
    void stop_watchers();

    // Event call back function
    void write_callback(ev::io &w, int r);
    void timeout_callback(ev::timer &w, int r);

    // The function will be called when it sent out all the data in the buffer
    function <void(char *, ssize_t)> wrote_event;
    // The function will be called when the connection is closed
    function <void(char *, ssize_t)> closed_event;
    // The function will be called when it is timeout or failed
    function <void(char *, ssize_t)> failed_event;

    // Set the timeout for the connection
    //  i: number of seconds, if it is ZERO, then no timeout
    void set_timeout(int i);
    // Starts sending data
    void start();
    // Resets the write pointer of the buffer and the timeout
    void reset();
    // Resets the buffer by assigning a new buffer and a new length
    //  buffer: pointer to a char buffer
    //  length: the expected length of the data that need to write
    void reset(char * buffer, ssize_t length);

    // Constructor
    //  Just for initialize, we do not know what the writer should write.
    //  We can define it later.
    //  loop: event loop
    //  descriptor: is the socket descriptor
    Async_Write(ev::default_loop *loop, int descriptor);

    // Constructor
    //  Creates a asynchronous socket writer
    //  loop: event loop
    //  descriptor: is the socket descriptor
    //  buffer: is the buffer that need to be sent
    //  length: expected length of the data
    Async_Write(ev::default_loop *loop, int descriptor, char * buffer, ssize_t length);

    // Destructor
    ~Async_Write();

};


#endif //PRPR_ASYNC_WRITE_HPP
