#ifndef SOCKETS_WITH_THREAD_GAME_H
#define SOCKETS_WITH_THREAD_GAME_H

#include <string>


// Suit le paterne "Observer" (https://refactoring.guru/design-patterns/observer)

class Game {
public:
    Game();
    int id;
    char* name;
    // void add_player(Client *client);

private:
    // std::vector<Client*> players;
};

#endif //SOCKETS_WITH_THREAD_GAME_H