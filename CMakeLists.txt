cmake_minimum_required(VERSION 3.9)
project(prpr)

set(CMAKE_CXX_STANDARD 11)
link_libraries(pthread)
link_libraries(crypto)
link_libraries(ev)

add_executable(main main.cpp main.hpp connection/Async_Accept.cpp connection/Async_Accept.hpp connection/Async_Connect.cpp connection/Async_Connect.hpp connection/Async_Read.cpp connection/Async_Read.hpp connection/Async_Write.cpp connection/Async_Write.hpp interface/Client_A.cpp interface/Client_A.hpp interface/Client_B.cpp interface/Client_B.hpp program/Client_Core.cpp program/Client_Core.hpp socks5/Channel.cpp socks5/Channel.hpp socks5/Handshake.cpp socks5/Handshake.hpp utility/Command_Listener.cpp utility/Command_Listener.hpp utility/Encryption.cpp utility/Encryption.hpp utility/Proxy_Peer.cpp utility/Proxy_Peer.hpp utility/Container.cpp utility/Container.hpp utility/Packet.cpp utility/Packet.hpp utility/Timeout_Listener.cpp utility/Timeout_Listener.hpp program/Socks_Core.cpp program/Socks_Core.hpp)

