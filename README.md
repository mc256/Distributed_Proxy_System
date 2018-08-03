![EECS4080 Poster](https://masterchan.me/eecs4080.png)

# Distributed Anti-Interference Proxy System

This is a EECS4080 Project at York University. 

Research Paper: **Junlin Chen and Uyen Trang Nguyen, A Robust Protocol for Circumventing Censoring Firewalls, 18th IEEE International Conference on Computer and Information Technology (CIT-2018), Halifax, Canada, July 30 - August 3, 2018.**

DOI 10.1109/Cybermatics_2018.2018.00299


## Compile & Run

You can compile this project using **cmake** or **makefile** in CentOS7. Please install [libev](https://github.com/enki/libev) before compile using 
```sh
yum install libev libev-devel
```
To run this program is very simple. But to be notices that, there are three mode of this program
- Client Application ``./a.out client``
- Server Application ``./a.out peer``
- SOCKS5 Server ``./a.out socks5``

That's it!

## Configuration

### Client Application
Please use the ``client_settings.txt``. You can use space or line break to separate these values

- Listen address
- Listen port
- Connection confirm password
- List of the interfaces
    - IP address
    - Port
    - Password


### Server Application
Please use the ``peer_settings.txt``. You can use space or line break to separate these values

- Connection confirm password
- SOCKS5 server address
- SOCKS5 server port
- Fake HTTP response (Should be a *.jpg image)
- List of the interfaces
    - Listen address
    - Port
    - Password (Password should be all different)
   
### SOCKS Server
Please use the ``socks_settings.txt``. You can use space or line break to separate these values

- Listen address
- Listen port


## Documentation

Source Code Structure

- **connection** <br />
This folder contains all the socket operation classes wrapped in a event-driven design.
    - **Async_Accept**: creates listener. It can accept connection.
    - **Async_Connect**: connects to a socket.
    - **Async_Read**: reads from a socket.
    - **Async_Write**: write to a socket.
- **interface** <br />
Lets define that there are two faces that the client application and the server application have. <br />
Such that <br />
``SOCKS Client <---> [A|Client Application|B] <===> [A|Server Application|B] <---> SOCKS Server <---> Internet``
    - **Client_A**: handles the connection to SOCKS Client
    - **Client_B**: handles the connection to Server Application
    - **Peer_A**: handles the connection to Client Application
    - **Peer_B**: handles the connection to SOCKS Server
- **program** <br />
We need a program that can manage two interface for a program. <br />
    - **Client_Core**: manages Client_A and Client_B (Client Application)
    - **Peer_Core**: manages Peer_A and Peer_B (Server Application)
    - **Socks_Core**: manages the SOCKS server, including all the files in ``./socks5/``.
- **socks5** <br />
Solely for SOCKS server <br />
    - **Channel**: forwards SOCKS5 proxy traffic
    - **Handshake**: takes care of the SOCKS5 handshake procedure.
- **utility** <br />
All the tools that need for this project<br />
    - **Command_Listener**: We can type command to the console. (But I did not implement this feature, UNUSED)
    - **Container**: keeps important value while the program is running.
    - **Encryption**: has all the encryption algorithm.
    - **File_Streamer**: reads the file and sends to the socket.
    - **Packet**: prepares the data being transmit over the proxy network.
    - **Proxy_Peer**: stores the configuration of the interfaces of the server application.
    - **Timeout_Listener**: listens a timeout event, refreshes the console, prints useful information.
    
    
    
    
    
    


**Please read detailed documentation in header files**
