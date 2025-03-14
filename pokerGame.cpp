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
std::vector<Card> flush(std::array<Card, N>& cards)
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
bool straight(const std::array<Card, N>& cards)
{
    std::array<int, Card::max_ranks+1> rankCounter {};

    for (auto& i : cards)
    {
        ++rankCounter.data()[i.rank];
        if (i.rank == 0)
        {
            ++rankCounter.data()[Card::max_ranks];
        }
    }

    int start {0};
    for (int i {0}; i<=Card::max_ranks; ++i)
    {
        if (rankCounter.data()[i] && i-start>=4)
        {
            return true;
        }
        if (rankCounter.data()[i])
        {
            continue;
        }
        start = i+1;
    }

    return false;
}

bool straight(const std::vector<Card>& cards)
{
    std::array<int, Card::max_ranks+1> rankCounter {};

    for (auto& i : cards)
    {
        ++rankCounter.data()[i.rank];
        if (i.rank == 0)
        {
            ++rankCounter.data()[Card::max_ranks];
        }
    }

    int start {0};
    for (int i {0}; i<=Card::max_ranks; ++i)
    {
        if (rankCounter.data()[i] && i-start>=4)
        {
            return true;
        }
        if (rankCounter.data()[i])
        {
            continue;
        }
        start = i+1;
    }

    return false;
}

template <std::size_t N>
bool straightFlush(const std::array<Card,N>& cards)
{
    std::array suitCounter {0, 0, 0, 0};
    
    for (auto& i : cards)
    {
        ++suitCounter.data()[i.suit];
    }

    Card::Suits mostSuit {};
    int mostSuitCount {0};

    for (auto i : Card::allSuits)
    {
        if (suitCounter.data()[i] > mostSuitCount)
        {
            mostSuitCount = suitCounter.data()[i];
            mostSuit = i;
        }
    }

    if (mostSuitCount < 5)
    {
        return false;
    }

    std::vector<Card> uniformSuit(static_cast<std::size_t>(mostSuitCount));

    std::size_t counter {0};
    for (auto i : cards)
    {
        if (i.suit == mostSuit)
        {
            uniformSuit[counter++] = i;
        }
    }

    return straight(uniformSuit);
}

int main()
{
    Deck deck {};
    deck.shuffle();

    return 0;
}