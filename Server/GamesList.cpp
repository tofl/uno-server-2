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

void GamesList::newGame() {
    Game* newGame = new Game();
    // allGames_->push_back(&newGame);
    allGames_.insert(allGames_.end(), newGame); // TODO check
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

char* GamesList::getNameByPosition(int i){
    return allGames_.at(i)->name;
}