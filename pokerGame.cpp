#include "deck.cpp"
#include <ranges>

namespace Settings
{
    int smallBlind {1};
    int bigBlind {2};
    int buyIn {200};
}

struct Player
{
    std::array<Card, 2> hand {};
    int chips {Settings::buyIn};
};

template <std::size_t N>
std::vector<Card> flush(std::array<Card, N> cards)
// vector output as it is moveable
{
    if (N<5)
    {
        return {};
    }

    std::vector<Card> bestHand(5);
    std::sort(cards.begin(),cards.end());
    
    Card::Suits prevSuit {Card::max_suits};
    std::size_t curPos {};
    for (const auto& i : std::views::reverse(cards))
    {
        if (i.suit != prevSuit)
        {
            curPos = 0;
            prevSuit = i.suit;
        }

        bestHand[curPos++] = i;
        if (curPos>=5)
        {
            return bestHand;
        }
    }
    
    return {};

}

template <std::size_t N>
std::vector<Card> straight(const std::array<Card, N>& cards)
{
    std::array<Card, Card::max_ranks+1> rankCounter {};
    std::vector<Card> bestHand {};
    bestHand.reserve(5);

    for (auto& i : rankCounter)
    {
        i = Card {Card::max_ranks,Card::max_suits};
    }

    for (const auto& i : cards)
    {
        rankCounter.data()[i.rank] = i;
        if (i.rank == 0)
        {
            rankCounter.data()[Card::max_ranks] = i;
        }
    }

    for (const auto& i : std::views::reverse(rankCounter))
    {
        if (i.rank != Card::max_ranks && i.suit != Card::max_suits)
        {
            bestHand.push_back(i);
        }
        else
        {
            bestHand.clear();
        }

        if (std::ssize(bestHand) == 5)
        {
            return bestHand;
        }

    }

    return {};
}

// template <std::size_t N>
// bool straightFlush(const std::array<Card,N>& cards)
// {
//     std::array suitCounter {0, 0, 0, 0};
    
//     for (auto& i : cards)
//     {
//         ++suitCounter.data()[i.suit];
//     }

//     Card::Suits mostSuit {};
//     int mostSuitCount {0};

//     for (auto i : Card::allSuits)
//     {
//         if (suitCounter.data()[i] > mostSuitCount)
//         {
//             mostSuitCount = suitCounter.data()[i];
//             mostSuit = i;
//         }
//     }

//     if (mostSuitCount < 5)
//     {
//         return false;
//     }

//     std::vector<Card> uniformSuit(static_cast<std::size_t>(mostSuitCount));

//     std::size_t counter {0};
//     for (auto i : cards)
//     {
//         if (i.suit == mostSuit)
//         {
//             uniformSuit[counter++] = i;
//         }
//     }

//     return straight(uniformSuit);
// }

int main()
{
    Deck deck {};
    // deck.shuffle();

    constexpr int drawnCards {7};
    std::array<Card,drawnCards> cards {};

    for (int i {0}; i<drawnCards; ++i)
    {
        cards.data()[i] = deck.dealCard();
        std::cout << cards.data()[i] << ' ';
    }

    std::cout << '\n';

    std::vector<Card> bestHand = straight(cards);

    for (auto i : bestHand)
    {
        std::cout << i << ' ';
    }

    return 0;
}