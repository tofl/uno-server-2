cmake_minimum_required(VERSION 3.20)
project(sockets_with_thread)

set(CMAKE_CXX_STANDARD 14)

include_directories(Server)

add_executable(sockets_with_thread
        Server/Client.cpp
        Server/Client.h
        Server/EndPoint.cpp
        Server/EndPoint.h
        Server/Ouput.cpp
        Server/Output.h
        Server/Semaphore.cpp
        Server/Semaphore.h
        Server/Server.cpp
        Server/StoppableServer.cpp
        Server/ThreadedSocket.cpp
        Server/ThreadedSocket.h
        Server/Client.cpp
        Server/Client.h
        Server/GamesList.cpp
        Server/GamesList.h
        Server/Game.cpp
        Server/Game.h
        )
