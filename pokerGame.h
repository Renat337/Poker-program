#pragma once

#include <iostream>
#include <vector>
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
    std::array<Card, 2> hand {};
    int chips {Settings::buyIn};
    Settings::Rankings handType {};
    std::vector<Card> bestHand {};
    bool isPlayer {false};

    Player(Card card1, Card card2)
    : hand {card1, card2} {}

    Player()
    {}

    bool operator<(const Player otherPlayer) const;
    bool operator==(const Player otherPlayer) const;
    void print() const;
};

template <typename T>
std::vector<Card> flush(const T& cards);

template <typename T>
std::vector<Card> straight(const T& cards);

template <typename T>
std::vector<Card> straightFlush(const T& cards);

template <typename T>
std::vector<Card> fourOfKind(const T& cards);

template <typename T>
std::vector<Card> threeOfKind(const T& cards);

template <typename T>
std::vector<Card> pair(const T& cards);

template <typename T>
std::vector<Card> fullHouse(const T& cards);

template <typename T>
std::vector<Card> twoPair(const T& cards);

template <typename T>
std::vector<Card> highCard(const T& cards);