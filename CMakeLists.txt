cmake_minimum_required(VERSION 3.9)
project(prpr)

set(CMAKE_CXX_STANDARD 11)
link_libraries(pthread)
link_libraries(crypto)
link_libraries(ev)

#add_executable(prpr main.cpp main.hpp Timeout_Listener.cpp Timeout_Listener.hpp Command_Listener.cpp Command_Listener.hpp Encryption.cpp Encryption.hpp Channel.cpp Channel.hpp Handshake.cpp Handshake.hpp W_Filter.cpp W_Filter.hpp R_Filter.cpp R_Filter.hpp Client_A.cpp Client_A.hpp Client_B.cpp Client_B.hpp Peer_A.cpp Peer_A.hpp Peer_B.cpp Peer_B.hpp Client_Core.cpp Client_Core.hpp Server_Accept.cpp Server_Accept.hpp Server_Connect.cpp Server_Connect.hpp)
add_executable(main main.cpp utility/Timeout_Listener.cpp utility/Command_Listener.cpp Encryption.cpp Channel.cpp Handshake.cpp W_Filter.cpp R_Filter.cpp Client_A.cpp Client_B.cpp  Peer_A.cpp  Peer_B.cpp  Client_Core.cpp  Server_Accept.hpp  Server_Connect.hpp Peer_Core.cpp Peer_Core.hpp Z_Test_Connect.cpp Z_Test_Connect.hpp connection/Async_Connect.cpp connection/Async_Connect.hpp)