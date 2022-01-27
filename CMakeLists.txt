cmake_minimum_required(VERSION 3.20)
project(sockets_with_thread)

set(CMAKE_CXX_STANDARD 14)

include_directories(Server)

add_executable(sockets_with_thread
        Game/Client.cpp
        Game/headerFiles/Client.h
        Server/EndPoint.cpp
        Server/headerFiles/EndPoint.h
        Server/Ouput.cpp
        Server/headerFiles/Output.h
        Server/Semaphore.cpp
        Server/headerFiles/Semaphore.h
        Server/Server.cpp
        Server/StoppableServer.cpp
        Server/ThreadedSocket.cpp
        Server/headerFiles/ThreadedSocket.h
        Game/Client.cpp
        Game/headerFiles/Client.h
        Game/GamesList.cpp
        Game/headerFiles/GamesList.h
        Game/Game.cpp
        Game/headerFiles/Game.h
        Game/utils.cpp
        Game/headerFiles/utils.h
        )
