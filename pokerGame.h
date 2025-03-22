#pragma once

#include <iostream>
#include <vector>
#include <utility>
#include "deck.h"

namespace Settings
{
    [[maybe_unused]] constexpr int smallBlind {1};
    [[maybe_unused]] constexpr int bigBlind {2};
    [[maybe_unused]] constexpr int buyIn {200};

    enum Rankings
    {
        high_card,
        pair,
        two_pair,
        three_kind,
        straight,
        flush,
        full_house,
        four_kind,
        straight_flush,

        max_rankings
    };

    enum GameStates
    {
        loss,
        draw,
        win,

        max_gamestates
    };

    [[maybe_unused]] static constexpr std::array<Rankings, max_rankings> allRankings {high_card, pair, two_pair, three_kind, straight,
                                                                    flush, full_house, four_kind, straight_flush};

    std::ostream& operator<<(std::ostream& out, const Rankings ranking);
}

struct Player
{
    std::pair<Card,Card> hand {};
    int chips {Settings::buyIn};
    Settings::Rankings handType {};
    std::vector<Card> bestHand {};
    bool isPlayer {false};

    Player(Card card1, Card card2)
    : hand{card1, card2} {}

    Player()
    {}

    bool operator<(const Player otherPlayer) const;
    bool operator==(const Player otherPlayer) const;
    void print() const;
};

std::vector<Card> flush(std::vector<Card> cards);
std::vector<Card> straight(std::vector<Card> cards);
std::vector<Card> straightFlush(std::vector<Card> cards);
std::vector<Card> fourOfKind(std::vector<Card> cards);
std::vector<Card> threeOfKind(std::vector<Card> cards);
std::vector<Card> pair(std::vector<Card> cards);
std::vector<Card> fullHouse(std::vector<Card> cards);
std::vector<Card> twoPair(std::vector<Card> cards);
std::vector<Card> highCard(std::vector<Card> cards);