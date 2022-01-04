#include "Game.h"

Game::Game(int gameId) {
    id = gameId;
}

void Game::addPlayer(Client *client) {
    players_.insert(players_.end(), client);
}

std::vector<Client*> Game::getClients() {
    return players_;
}