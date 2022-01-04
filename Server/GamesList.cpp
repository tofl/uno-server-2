#include "GamesList.h"
#include "Game.h"

GamesList* GamesList::singleton_ = nullptr;

GamesList* GamesList::GetInstance()
{
    if (singleton_ == nullptr) {
        singleton_ = new GamesList();
    }
    return singleton_;
}

GamesList::GamesList()
{
}

GamesList::~GamesList() {
    for (Game* game: allGames_) {
        delete game;
    }
}

int GamesList::newGame() {
    int newId = allGames_.size();
    Game* newGame = new Game(newId);
    allGames_.insert(allGames_.end(), newGame);
    return newId;
}

std::vector<Game*> GamesList::getAllGames() {
    return allGames_;
}

bool GamesList::hasGame(int gameId) {
    for (Game *game: allGames_) {
        if (game->id == gameId) {
            return true;
        }
    }

    return false;
}

Game* GamesList::getGame(int gameId) {
    for (Game *game: allGames_) {
        if (game->id == gameId) {
            return game; // TODO vérifier que le pointeur est renvoyé
        }
    }
}
