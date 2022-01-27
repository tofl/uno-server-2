#include <string>
#include <vector>
#include "headerFiles/utils.h"

namespace utils {
    std::string formatCardsToUserResponse(std::vector<std::string> cards) {
        std::string cardListString = "";
        for (std::string card : cards) {
            cardListString += card + ",";
        }

        if (cardListString.back() == ',') {
            cardListString.pop_back();
        }

        return cardListString;
    }
}

/*
std::string formatCardsToUserResponse() {
    std::string cardListString = "";
    for (std::string card : cards) {
        cardListString += card + ",";
    }

    if (cardListString.back() == ',') {
        cardListString.pop_back();
    }

    return cardListString;
}
 */