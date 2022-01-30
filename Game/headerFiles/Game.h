#ifndef SOCKETS_WITH_THREAD_GAME_H
#define SOCKETS_WITH_THREAD_GAME_H

#include <string>
#include "Client.h"


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
    std::string getCardNumber(std::string);
    std::string setCurrentNumber(std::string);
    bool actionIsLegal(std::string);
    Client* getPlayer(int);
    void removePlayer(int);
    void setCurrentPlayer(Client*);
    Client* getCurrentPlayer();
    Client* getNextPlayer();

private:
    std::vector<Client*> players_;
    std::vector<std::string> availableCards_;
    std::string lastCard_;
    std::string currentColor_;
    std::string currentNumber_;
    Client* currentPlayer_;
};

#endif //SOCKETS_WITH_THREAD_GAME_H