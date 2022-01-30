#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <iostream>
#include <thread>
#include <string>
#include "headerFiles/ThreadedSocket.h"
#include "headerFiles/Client.h"
#include "headerFiles/Output.h"
#include "headerFiles/GamesList.h"
#include "headerFiles/utils.h"

#ifdef _WIN32
Client::Client(int id, SOCKET socket, const int MAXDATASIZE) : ThreadedSocket(socket, false, MAXDATASIZE), id(id)
{
	buffer = new char[MAXDATASIZE];

	char numstr[21]; // enough to hold all numbers up to 64-bits
	sprintf(numstr, "%d", id);
	std::string prefix = std::string(std::string(std::string("[CLIENT_") + numstr + std::string("] ")));
	output_prefix = (char*)malloc(strlen(prefix.c_str()) + 1);
	strcpy(output_prefix, prefix.c_str());
}
#else
Client::Client(int id, int socket, const int MAXDATASIZE) : ThreadedSocket(socket, false, MAXDATASIZE), id(id)
{
	buffer = new char[MAXDATASIZE];
    signal(SIGPIPE, SIG_IGN);

    char numstr[21]; // enough to hold all numbers up to 64-bits
    sprintf(numstr, "%d", id);
    std::string prefix = std::string(std::string(std::string("[CLIENT_") + numstr + std::string("] ")));
    output_prefix = (char*)malloc(strlen(prefix.c_str()) + 1);
    strcpy(output_prefix, prefix.c_str());
}
#endif

Client::~Client()
{
	ThreadedSocket::~ThreadedSocket();
	delete[] buffer;
	free(output_prefix);
}

int Client::getId() {
    return id;
}

void Client::addCardToHand(std::string card) {
    cards.push_back(card);
}

bool Client::send_message(const char* buffer)
{
	if (socket_ == 0 || !is_alive)
		return false;

    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt (socket_, SOL_SOCKET, SO_ERROR, &error, &len);

    if (retval != 0) {
        /* there was a problem getting the error code */
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        return false; // TODO retirer le client de la liste de joueurs (?)
    }

    if (error != 0) {
        /* socket has a non zero error status */
        fprintf(stderr, "socket error: %s\n", strerror(error));
        return false; // TODO retirer le client de la liste de joueurs (?)
    }

    if (send(socket_, buffer, strlen(buffer), 0) == -1) {
		Output::GetInstance()->print_error(output_prefix, "Error while sending message to client ");
		return false;
	}

	return true;
}

int Client::recv_message()
{
	if (socket_ == 0 || !is_alive)
		return -1;

	int length;
	if ((length = recv(socket_, buffer, MAXDATASIZE, 0)) == -1) {
		Output::GetInstance()->print_error(output_prefix, "Error while receiving message from client ");
		return length;
	}

	// Suppression des retours chariots (\n et \r)
	while (length > 0 && (buffer[length - 1] == '\n' || buffer[length - 1] == '\r'))
		length--;
	// Ajout de backslash zero a la fin pour en faire une chaine de caracteres
	if (length >= 0 && length < MAXDATASIZE)
		buffer[length] = '\0';

	return length;
}

void Client::end_thread()
{
	if (!is_alive) {
        return;
    }

    leaveGameOnDisconnect();

	// Sending close connection to client
	send_message("CONNECTION_CLOSED");

	ThreadedSocket::end_thread();
}

std::vector<std::string> Client::getCards() {
    return cards;
}

int Client::cardIndex(std::string cardName) {
    int cardIndex = -1;
    for (int i = 0; i < cards.size(); i++) {
        if (cards[i] == cardName) {
            cardIndex = i;
        }
    }

    return cardIndex;
}

void Client::removeCardFromHand(int cardIndex) {
    cards.erase(cards.begin() + cardIndex);
}

bool Client::leaveGameOnDisconnect() {
    if (currentGameId) {
        Game *game = GamesList::GetInstance()->getGame(currentGameId);

        // Quitter la partie
        game->removePlayer(id);
        Output::GetInstance()->print(output_prefix, "Removing client from game.\n");

        // Supprimer la partie s'il n'y a plus de joueur
        if (game != NULL && game->getClients().size() == 0) {
            GamesList::GetInstance()->removeGame(currentGameId);
        }
        return true;
    }
    return false;
}

void Client::execute_thread()
{
	int length;
	time_t time_value;
	struct tm* time_info;

	Output::GetInstance()->print(output_prefix, "Thread client starts with id=", id, ".\n");

	// Boucle infinie pour le client
    while (1) {

        if (socket_ == 0 || !is_alive)
            return;

        // On attend un message du client
        if ((length = recv_message()) == -1) {
            break;
        }

        // Affichage du message
        Output::GetInstance()->print(output_prefix, "Message received : ", buffer, "\n");

        // On considère que l'action menée est illégale jusqu'à preuve du contraire
        bool moveWasSuccessful = false;

        if (strcmp(buffer, "DISCONNECT") == 0) {
            leaveGameOnDisconnect();
            break;
        }
        else {
            // On recupere l'heure et la date
            time(&time_value);
            time_info = localtime(&time_value);

            std::string fullCmd(buffer);
            std::size_t sepPos = fullCmd.find(";");
            std::string cmdName;
            std::string cmdBody;
            if (sepPos != std::string::npos) {
                cmdName = fullCmd.substr(0, sepPos);
                cmdBody = fullCmd.substr(sepPos + 1);
            } else {
                cmdName = fullCmd;
            }

            // Traitement du message reçu
            if (cmdName == "NEW_GAME") {
                int newGameId = GamesList::GetInstance()->newGame();
                // sprintf(buffer, "%s", std::to_string(newGameId).c_str());
                send_message(std::to_string(newGameId).c_str());
            }

            else if (cmdName == "GET_GAMES") {
                // Get all games
                std::string gamesList = "";
                for (Game *game: GamesList::GetInstance()->getAllGames()) {
                    gamesList += std::to_string(game->id) + "-" + std::to_string(game->getClients().size()) + ",";
                }
                if (gamesList == "") {
                    gamesList = "no game was found";
                } else {
                    gamesList.pop_back();
                }

                send_message(("games list" + gamesList).c_str());
            }

            else if (cmdName == "CURRENT_GAME_INFO") {
                if (!currentGameId) {
                    send_message("you must join a game");
                } else {
                    Game *game = GamesList::GetInstance()->getGame(currentGameId);
                    std::vector<Client*> clients = game->getClients();

                    std::string returnedList;
                    for (Client* client : clients) {
                        std::string clientIdAndCards = std::to_string(client->id) + "-" + std::to_string(client->getCards().size()) + ",";
                        returnedList += clientIdAndCards;
                    }

                    if (returnedList == "") {
                        returnedList = "no players were found";
                    } else {
                        returnedList.pop_back();
                    }

                    send_message(("clients and nb cards list:" + returnedList).c_str());
                    send_message(("current card:" + game->getLastCard()).c_str());
                }
            }

            else if (cmdName == "JOIN") {
                Game *game = GamesList::GetInstance()->getGame(std::stoi(cmdBody));
                game->addPlayer(this);
                currentGameId = game->id;

                std::string clientList = "";
                for (Client *client: game->getClients()) {
                    clientList += std::to_string(client->id) + ",";
                }
                if (clientList == "") {
                    clientList = "no game was found";
                } else {
                    clientList.pop_back();
                }

                // On envoie à chaque client la liste de nouveaux joueurs et leurs cartes
                for (Client *client: game->getClients()) {
                    client->send_message(("client list:" + clientList).c_str());
                }

                // Sélectionner 7 cartes
                for (int i = 0; i < 7; i++) {
                    std::string card = game->pickRandomCard();
                    cards.push_back(card);
                }

                std::string cardListString = utils::formatCardsToUserResponse(cards);
                send_message(("cards list:" + cardListString).c_str());
            }

            else if (cmdName == "PLAY_PUT_DOWN") {
                Game *game = GamesList::GetInstance()->getGame(currentGameId);

                int cardI = cardIndex(cmdBody);

                // Vérifier que c'est au tour du joueur de jouer
                if (game->getCurrentPlayer()->getId() != id) {
                    send_message("not your turn to play");
                } else if (cardI == -1) { // vérifier que le joueur a la carte
                    // return error : the player doesn't have the card
                    send_message("card does not exist");
                } else {
                    // Accepter n'importe quelle carte au tout début du jeu
                    // TODO ajouter éventuellement des règles pour interdire certaines cartes (?)
                    if (game->getLastCard() == "") {
                        // On retire la carte de la main du joueur
                        removeCardFromHand(cardI);

                        // On pose la carte
                        game->putDownCard(cmdBody);

                        // On définit la couleur
                        game->setCurrentColor(cmdBody);

                        moveWasSuccessful = true;
                    } else {
                        // vérifier au cas par cas la légalité de l'action menée par le joueur
                        if (!game->actionIsLegal(cmdBody)) {
                            send_message("Action not legal");
                        } else {
                            moveWasSuccessful = true;
                            removeCardFromHand(cardI);
                            game->putDownCard(cmdBody);
                            game->setCurrentColor(cmdBody);
                            send_message("successful");
                        }
                    }

                    if (moveWasSuccessful) {
                        // notifier tous les joueurs du nouvel état du jeu de carte (renvoyer la carte qui a été posée)
                        std::string buf("new card:");
                        buf.append(cmdBody);

                        // Déterminer la prochain joueur à jouer
                        Client* nextPlayer = game->getNextPlayer();

                        for (Client *client: game->getClients()) {
                            client->send_message(buf.c_str());
                            client->send_message(("next player:" + std::to_string(nextPlayer->id)).c_str());
                        }

                        game->setCurrentPlayer(nextPlayer);
                    }
                }
            }

            else if (strcmp(buffer, "PLAY_PICK") == 0) {
                Game *game = GamesList::GetInstance()->getGame(currentGameId);
                std::string newCard = game->pickRandomCard();
                cards.push_back(newCard);

                // On passe au joueur suivant
                Client* nextPlayer = game->getNextPlayer();

                for (Client *client: game->getClients()) {
                    client->send_message(("next player:" + std::to_string(nextPlayer->id)).c_str());
                }

                game->setCurrentPlayer(nextPlayer);
            }

            // TODO finir ou supprimer
            else if (strcmp(buffer, "PLAY_UNO") == 0) {
                if (cards.size() == 1) {
                    send_message("Yes!");
                } else {
                    send_message("No!");
                }
            }

            else if (strcmp(buffer, "PLAY_CONTRE_UNO") == 0) {
                Game *game = GamesList::GetInstance()->getGame(currentGameId);

                Client* targetedPlayer = game->getPlayer(std::stoi(cmdBody));

                std::string card1 = game->pickRandomCard();
                std::string card2 = game->pickRandomCard();
                if (targetedPlayer->cards.size() == 1) {
                    targetedPlayer->addCardToHand(card1);
                    targetedPlayer->addCardToHand(card2);
                }
            }

            else if (cmdName == "GET_CARDS_LIST") {
                send_message(("cards list:" + utils::formatCardsToUserResponse(cards)).c_str());
            }

            else {
                // sprintf(buffer, "%s is not recognized as a valid command", buffer);
                Output::GetInstance()->print("Received an unrecognized command '", buffer, "'\n");
                if (!send_message("Command not recognised")) {
                    break;
                }
            }

            if (socket_ == NULL || !is_alive)
                return;
        }
    }

	end_thread();
}