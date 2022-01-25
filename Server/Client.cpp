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
#include "ThreadedSocket.h"
#include "Client.h"
#include "Output.h"
#include "GamesList.h"

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
	if (!is_alive)
		return;

	// Sending close connection to client
	send_message("CONNECTION_CLOSED");

	ThreadedSocket::end_thread();
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

        // if (socket_ == 0 || !is_alive)
        //	return;

        // Affichage du message
        Output::GetInstance()->print(output_prefix, "Message received : ", buffer, "\n");

        // On considère que l'action menée est illégale jusqu'à preuve du contraire
        bool moveWasSuccessful = false;

        if (strcmp(buffer, "DISCONNECT") == 0) {
            break;
        } else {
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
            } else if (cmdName == "GET_GAMES") {
                // Get all games
                std::string gamesList = "";
                for (Game *game: GamesList::GetInstance()->getAllGames()) {
                    gamesList += std::to_string(game->id) + ",";
                }
                if (gamesList == "") {
                    gamesList = "no game was found";
                } else {
                    gamesList.pop_back();
                }

                send_message(gamesList.c_str());
            } else if (cmdName == "JOIN") {
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
                    client->send_message(clientList.c_str());
                }

                // Sélectionner 7 cartes
                std::string cardListString = "";
                for (int i = 0; i < 7; i++) {
                    std::string card = game->pickRandomCard();
                    cards.push_back(card);
                    cardListString += card + ",";
                }
                cardListString.pop_back();

                send_message(cardListString.c_str());
            } else if (strcmp(buffer, "USER_DETAILS") == 0) {}  // TODO nope

            else if (cmdName == "PLAY_PUT_DOWN") {
                Game *game = GamesList::GetInstance()->getGame(currentGameId);

                // vérifier que le joueur a la carte
                int cardI = cardIndex(cmdBody);
                if (cardI == -1) {
                    // return error : the player doesn't have the card
                    send_message("card does not exist");
                } else {
                    // send_message(std::to_string(cardI).c_str());

                    // Accepter n'importe quelle carte au tout début du jeu
                    // TODO ajouter éventuellement des règles pour interdire certaines cartes (?)
                    if (game->getLastCard() == "") {
                        // On retire la carte de la main du joueur
                        removeCardFromHand(cardI);

                        // On pose la carte
                        game->putDownCard(cmdBody);

                        // On définit la couleur
                        game->setCurrentColor(cmdBody);
                    } else {
                        // vérifier au cas par cas la légalité de l'action menée par le joueur
                        if (!game->actionIsLegal(cmdBody)) {
                            send_message("Action not legal");
                        } else {
                            moveWasSuccessful = true;
                            removeCardFromHand(cardI);
                            game->putDownCard(cmdBody);
                            game->setCurrentColor(cmdBody);
                        }
                    }

                    if (moveWasSuccessful) {
                        // notifier tous les joueurs du nouvel état du jeu de carte (renvoyer la carte qui a été posée)
                        std::string buf("New card;");
                        buf.append(cmdBody);
                        for (Client *client: game->getClients()) {
                            client->send_message(buf.c_str());
                        }
                    }
                }
            } else if (strcmp(buffer, "PLAY_PICK") == 0) {
                Game *game = GamesList::GetInstance()->getGame(currentGameId);
                game->pickRandomCard();
            } else if (strcmp(buffer, "PLAY_UNO") == 0) {}

            else if (strcmp(buffer, "PLAY_CONTRE_UNO") == 0) {}

            else if (strcmp(buffer, "RESPONSE_UNO") == 0) {} // TODO nope

            else if (strcmp(buffer, "GAME_STATUS") == 0) {} // TODO nope

            else if (strcmp(buffer, "ERROR") == 0) {} // TODO nope

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