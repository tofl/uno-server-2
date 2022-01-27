#ifndef SOCKETS_WITH_THREAD_GAMESLIST_H
#define SOCKETS_WITH_THREAD_GAMESLIST_H

#include <stdlib.h>
#include <vector>
#include "Game.h"

class GamesList {
private:
    GamesList();
    static GamesList* singleton_;
    std::vector<Game*> allGames_;
    ~GamesList();


public:
    GamesList(GamesList& other) = delete;
    void operator=(const GamesList & gamesList) = delete; // TODO see in details
    static GamesList *GetInstance();

    int newGame();
    std::vector<Game*> getAllGames();
    Game* getGame(int gameId);
    bool hasGame(int gameId);
    void removeGame(int gameId);
};


#endif //SOCKETS_WITH_THREAD_GAMESLIST_H
