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
    std::string pickRandomCard();

private:
    std::vector<Client*> players_;
    std::vector<std::string> availableCards_;
};

#endif //SOCKETS_WITH_THREAD_GAME_H