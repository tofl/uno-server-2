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


public:
    GamesList(GamesList& other) = delete;
    void operator=(const GamesList & gamesList) = delete; // TODO see in details
    static GamesList *GetInstance();

    void newGame();
    std::vector<Game*> getAllGames();
    Game* getGame(int gameId);
    char* getNameByPosition(int);
    bool hasGame(int gameId);
};


#endif //SOCKETS_WITH_THREAD_GAMESLIST_H
