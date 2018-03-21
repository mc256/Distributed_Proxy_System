//
// Created by mc on 3/20/18.
//

#ifndef PRPR_FILE_STREAMER_HPP
#define PRPR_FILE_STREAMER_HPP

#include "../main.hpp"

class File_Streamer {
private:
    int socket_id;
    ifstream file_stream;


public:
    char send_buffer[MAX_BUFFER_SIZE];
    size_t send_buffer_size;

    Async_Write *write_handler;

    void start();

    void file_start();

    File_Streamer(ev::default_loop *loop, int socket_id, string filename);

    ~File_Streamer();
};

/*
 * Fle_Streamer * fs = new File_Streamer(c->loop, d, "../test.jpg2");
 * fs->start();
 * */

#endif //PRPR_FILE_STREAMER_HPP
