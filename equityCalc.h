#pragma once

#include <utility>
#include <vector>
#include <functional>
#include "pokerGame.h"
#include "deck.h"

std::pair<int, std::vector<Player>> getHands(int numPlayers);
std::pair<int, std::vector<Card>> getCommunalCards();

struct HandFunction
{
    using HandEvaluator = std::function<std::vector<Card>(std::vector<Card>)>;

    HandEvaluator function;
    Settings::Rankings ranking;
};

Settings::GameStates checkWinner(std::vector<Player>& players, std::vector<Card>& communalCards);
void runTests(int tries);