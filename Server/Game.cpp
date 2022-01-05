#include "Game.h"

Game::Game(int gameId) {
    id = gameId;

    // Création du jeu de cartes
    availableCards_ = {
        "G0",
        "G1",
        "G1",
        "G2",
        "G2",
        "G3",
        "G3",
        "G4",
        "G4",
        "G5",
        "G5",
        "G6",
        "G6",
        "G7",
        "G7",
        "G8",
        "G8",
        "G9",
        "G9",
        "Y0",
        "Y1",
        "Y1",
        "Y2",
        "Y2",
        "Y3",
        "Y3",
        "Y4",
        "Y4",
        "Y5",
        "Y5",
        "Y6",
        "Y6",
        "Y7",
        "Y7",
        "Y8",
        "Y8",
        "Y9",
        "Y9",
        "R0",
        "R1",
        "R1",
        "R2",
        "R2",
        "R3",
        "R3",
        "R4",
        "R4",
        "R5",
        "R5",
        "R6",
        "R6",
        "R7",
        "R7",
        "R8",
        "R8",
        "R9",
        "R9",
        "B0",
        "B1",
        "B1",
        "B2",
        "B2",
        "B3",
        "B3",
        "B4",
        "B4",
        "B5",
        "B5",
        "B6",
        "B6",
        "B7",
        "B7",
        "B8",
        "B8",
        "B9",
        "B9",
        "G_+2", // Le joueur suivant pioche 2 cartes
        "G_+2",
        "Y_+2",
        "Y_+2",
        "R_+2",
        "R_+2",
        "B_+2",
        "B_+2",
        "G_SKIP", // Cartes "passer" (le joueur suivant passe son tour)
        "G_SKIP",
        "Y_SKIP",
        "Y_SKIP",
        "R_SKIP",
        "R_SKIP",
        "B_SKIP",
        "B_SKIP",
        "G_INVERT", // Inverstion du sens
        "G_INVERT",
        "Y_INVERT",
        "Y_INVERT",
        "R_INVERT",
        "R_INVERT",
        "B_INVERT",
        "B_INVERT",
        "JOKER", // Permet de choisir la couleur
        "JOKER",
        "JOKER",
        "JOKER",
        "SUPERJOKER", // Choisir la couleur et oblige le joueur suivant à piocher 4 cartes
        "SUPERJOKER",
        "SUPERJOKER",
        "SUPERJOKER"
    };
}

void Game::addPlayer(Client *client) {
    players_.insert(players_.end(), client);
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