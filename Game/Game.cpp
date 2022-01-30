#include "headerFiles/Game.h"

Game::Game(int gameId) {
    id = gameId;

    // Création du jeu de cartes
    availableCards_ = { // G = vert, Y = jaune, R = rouge, B = bleu
        "G_Zero",
        "G_One",
        "G_One",
        "G_Two",
        "G_Two",
        "G_Three",
        "G_Three",
        "G_Four",
        "G_Four",
        "G_Five",
        "G_Five",
        "G_Six",
        "G_Six",
        "G_Seven",
        "G_Seven",
        "G_Eight",
        "G_Eight",
        "G_Nine",
        "G_Nine",
        "Y_0",
        "Y_One",
        "Y_One",
        "Y_Two",
        "Y_Two",
        "Y_Three",
        "Y_Three",
        "Y_Four",
        "Y_Four",
        "Y_Five",
        "Y_Five",
        "Y_Six",
        "Y_Six",
        "Y_Seven",
        "Y_Seven",
        "Y_Eight",
        "Y_Eight",
        "Y_Nine",
        "Y_Nine",
        "R_0",
        "R_One",
        "R_One",
        "R_Two",
        "R_Two",
        "R_Three",
        "R_Three",
        "R_Four",
        "R_Four",
        "R_Five",
        "R_Five",
        "R_Six",
        "R_Six",
        "R_Seven",
        "R_Seven",
        "R_Eight",
        "R_Eight",
        "R_Nine",
        "R_Nine",
        "B_0",
        "B_One",
        "B_One",
        "B_Two",
        "B_Two",
        "B_Three",
        "B_Three",
        "B_Four",
        "B_Four",
        "B_Five",
        "B_Five",
        "B_Six",
        "B_Six",
        "B_Seven",
        "B_Seven",
        "B_Eight",
        "B_Eight",
        "B_Nine",
        "B_Nine",
        "G_DrawTwo", // Le joueur suivant pioche 2 cartes
        "G_DrawTwo",
        "Y_DrawTwo",
        "Y_DrawTwo",
        "R_DrawTwo",
        "R_DrawTwo",
        "B_DrawTwo",
        "B_DrawTwo",
        "G_SKIP", // Cartes "passer" (le joueur suivant passe son tour)
        "G_SKIP",
        "Y_SKIP",
        "Y_SKIP",
        "R_SKIP",
        "R_SKIP",
        "B_SKIP",
        "B_SKIP",
        "G_REVERSE", // Inverstion du sens
        "G_REVERSE",
        "Y_REVERSE",
        "Y_REVERSE",
        "R_REVERSE",
        "R_REVERSE",
        "B_REVERSE",
        "B_REVERSE",
        "ChangeColor", // Permet de choisir la couleur TODO peut être que la couleur choisie pourrait être annotée au nom de la carte (style Y_JOKER pour choisir du yellow)
        "ChangeColor",
        "ChangeColor",
        "ChangeColor",
        "DrawFour", // Choisir la couleur et oblige le joueur suivant à piocher 4 cartes
        "DrawFour",
        "DrawFour",
        "DrawFour"
    };

    lastCard_ = "";
    currentColor_ = "";
}

void Game::addPlayer(Client *client) {
    players_.insert(players_.end(), client);

    // Le premier joueur à rejoindre la partie commence à jouer
    if (players_.size() == 1) {
        currentPlayer_ = players_[0];
    }
}

std::vector<Client*> Game::getClients() {
    return players_;
}

std::string Game::pickRandomCard() {
    int randomIndex = rand() % availableCards_.size();
    std::string returnedCard = availableCards_[randomIndex];
    availableCards_.erase(availableCards_.begin() + randomIndex);
    return returnedCard;
}

void Game::putDownCard(std::string card) {
    lastCard_ = card;
}

std::string Game::getLastCard() {
    return lastCard_;
}

std::string Game::getCardColor(std::string cardName) {
    std::string firstLetter = std::to_string(cardName[0]);

    if (firstLetter != "G" || firstLetter != "Y" || firstLetter != "R" || firstLetter != "B") {
        return "";
    }

    return firstLetter;
}

void Game::setCurrentColor(std::string cardName) {
    currentColor_ = getCardColor(cardName);
}

std::string Game::getCurrentColor() {
    return currentColor_;
}

bool Game::actionIsLegal(std::string cardName) {
    // TODO implémenter complètement
    std::string cardColor = getCardColor(cardName);

    // La carte n'a pas de couleur, elle peut être jouée à tout moment (?)
    if (cardColor == "") {
        return true;
    }

    if (cardColor == currentColor_) {
        return true;
    }
    return false;
}

Client* Game::getPlayer(int playerId) {
    for (Client* c : players_) {
        if (c->getId() == playerId) {
            return c;
        }
    }
    return NULL;
}

void Game::removePlayer(int playerId) {
    // Récupérer la position du joueur dans la liste
    int playerPosition;
    int i = 0;

    for (Client* cl : players_) {
        if (cl->getId() == playerId) {
            playerPosition = i;
        }
        i++;
    }

    players_.erase(players_.begin() + playerPosition);
}

void Game::setCurrentPlayer(Client* client) {
    currentPlayer_ = client;
}

Client* Game::getCurrentPlayer() {
    return currentPlayer_;
}

Client* Game::getNextPlayer() {
    // Récupérer la position (index) du joueur actuel dans la liste des joueurs
    int playerPosition;
    int i = 0;
    for (Client* player : players_) {
        if (currentPlayer_->getId() == player->getId()) {
            playerPosition = i;
        }
        i++;
    }

    if (playerPosition == players_.size() - 1) {
        playerPosition = 0;
    } else {
        playerPosition++;
    }

    return players_[playerPosition];
}