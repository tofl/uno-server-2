#ifndef SOCKETS_WITH_THREAD_GAME_H
#define SOCKETS_WITH_THREAD_GAME_H

#include <string>
#include "Client.h"


// Suit le paterne "Observer" (https://refactoring.guru/design-patterns/observer)

class Game {
public:
    Game(int gameId);
    int id;
    void addPlayer(Client *client);
    std::vector<Client*> getClients();

private:
    std::vector<Client*> players_;
};

#endif //SOCKETS_WITH_THREAD_GAME_H