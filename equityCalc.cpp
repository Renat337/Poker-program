#include <iostream>
#include <cassert>
#include <vector>
#include <ranges>
#include <functional>
#include <utility>
#include "deck.h"
#include "pokerGame.h"
#include "equityCalc.h"

std::pair<int, std::vector<Player>> getHands(int numPlayers)
{
    std::cout << "Do you want to customise the player's hands? Type y or n: ";
    char controlHandsChar {};
    std::cin >> controlHandsChar;
    bool controlHands {controlHandsChar == 'y'};

    std::vector<Player> players (static_cast<std::size_t>(numPlayers));

    int numHands {};
    if (controlHands)
    {
        std::cout << "How many hands do you want to input? ";
        std::cin >> numHands;
        assert(numHands <= numPlayers);

        for (int i {0}; i<numHands; ++i)
        {
            Card card1 {};
            Card card2 {};
            std::cout << "Input the hand for player " << i+1 << '\n';
            std::cin >> card1 >> card2;

            players.data()[i] = Player {card1, card2};
        }
    }

    players.data()[0].isPlayer = true;

    return {numHands, std::move(players)};
}

std::pair<int, std::vector<Card>> getCommunalCards()
{
    std::cout << "Do you want to control the communal cards? Type y or n: ";
    char controlCommunalChar {};
    std::cin >> controlCommunalChar;
    bool controlCommunal {controlCommunalChar == 'y'};

    std::vector<Card> communalCards(7);

    int numCommunal {};
    if (controlCommunal)
    {
        std::cout << "How many communal cards do you want to input? ";
        std::cin >> numCommunal;
        assert(numCommunal <= 5);

        for (int i {0}; i<numCommunal; ++i)
        {
            std::cout << "Input communal card " << i << '\n';
            std::cin >> communalCards.data()[i];
        }
    }
    
    return {numCommunal, std::move(communalCards)};
}

Settings::GameStates checkWinner(std::vector<Player>& players, std::vector<Card>& communalCards)
{
    static const std::vector<HandFunction> handFunctions
    {
    {&straightFlush, Settings::straight_flush},
    {&fourOfKind, Settings::four_kind},
    {&fullHouse, Settings::full_house},
    {&flush, Settings::flush},
    {&straight, Settings::straight},
    {&threeOfKind, Settings::three_kind},
    {&twoPair, Settings::two_pair},
    {&pair, Settings::pair},
    {&highCard, Settings::high_card}
    };

    Settings::Rankings bestHandType {Settings::high_card};
    std::vector<Player> winning {};

    for (auto& player : players)
    {
        communalCards.data()[5] = player.hand.data()[0];
        communalCards.data()[6] = player.hand.data()[1];

        for (const auto& handFunc : handFunctions)
        {
            auto bestHand = handFunc.function(communalCards);
            if (!bestHand.empty())
            {
            player.bestHand = std::move(bestHand);
            player.handType = handFunc.ranking;

            if (player.handType > bestHandType)
            {
                bestHandType = player.handType;
                winning.clear();
                winning.push_back(player);
            }
            else if (player.handType == bestHandType)
            {
                winning.push_back(player);
            }
            break;
            }
        }
    }

    std::sort(winning.begin(), winning.end());

    for (int i {0}; i<std::ssize(winning); ++i)
    {
        if (winning.data()[i].isPlayer && i==std::ssize(winning)-1)
        {
            return Settings::win;
        }
        if (winning.data()[i].isPlayer && winning.data()[i] == winning.back())
        {
            return Settings::draw;
        }
        if (winning.data()[i].isPlayer)
        {
            return Settings::loss;
        }
    }

    return Settings::loss;

}

void runTests(int tries)
{
    int wins {0};
    int draws {0};
    
    Deck deck {};

    std::cout << "How many players do you want? ";
    int numPlayers {};
    std::cin >> numPlayers;

    auto [numHands, players] = getHands(numPlayers);

    auto [numCommunal, communalCards] = getCommunalCards();

    std::vector<Card> usedCards {};
    usedCards.reserve(static_cast<std::size_t>(numHands*2+numCommunal));
    for (std::size_t i {0}; i < static_cast<std::size_t>(numHands); ++i)
    {
        usedCards.push_back(players.data()[i].hand[0]);
        usedCards.push_back(players.data()[i].hand[1]);
    }
    for (std::size_t i {0}; i < static_cast<std::size_t>(numCommunal); ++i)
    {
        usedCards.push_back(communalCards.data()[i]);
    }

    for (int n {0}; n<tries; ++n)
    {
        deck.shuffle(usedCards);

        for (int i {numHands}; i<numPlayers; ++i)
        {
            players.data()[i] = Player {deck.dealCard(), deck.dealCard()};
        }
        players.data()[0].isPlayer = true;

        for (int i {numCommunal}; i<5; ++i)
        {
            communalCards.data()[i] = deck.dealCard();
        }

        auto gameRes {checkWinner(players, communalCards)};

        if (gameRes == Settings::win)
        {
            ++wins;
        }
        else if (gameRes == Settings::draw)
        {
            ++draws;
        }
    }

    std::cout << "Wins: " << wins << '\n';
    std::cout << "Draws: " << draws << '\n';
    std::cout << "Losses: " << tries-wins-draws << '\n';
    std::cout << "Win rate: " << 100 * static_cast<double>(wins)/tries << "%\n";
    std::cout << "Draw rate: " << 100 * static_cast<double>(draws)/tries << "%\n";
    std::cout << "Loss rate: " << 100 * static_cast<double>(tries-wins-draws)/tries << "%\n";

}

int main()
{
    runTests(1000);

    return 0;
}