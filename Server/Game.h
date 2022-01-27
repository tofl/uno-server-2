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
    std::string getLastCard();
    void putDownCard(std::string);
    void setCurrentColor(std::string);
    std::string getCardColor(std::string);
    std::string getCurrentColor();
    bool actionIsLegal(std::string);
    Client* getPlayer(int);

private:
    std::vector<Client*> players_;
    std::vector<std::string> availableCards_;
    std::string lastCard_;
    std::string currentColor_;
};

#endif //SOCKETS_WITH_THREAD_GAME_H