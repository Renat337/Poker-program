#include "deck.cpp"

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
bool flush(const std::array<Card, N>& cards)
{
    std::array suitCounter {0, 0, 0, 0};
    
    for (auto& i : cards)
    {
        ++suitCounter.data()[i.suit];
    }

    for (auto& i : suitCounter)
    {
        if (i >= 5)
        {
            return true;
        }
    }

    return false;
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

    constexpr std::size_t numDraws {7};
    std::array<Card, numDraws> cards{};

    for (std::size_t i {0}; i<numDraws; ++i)
    {
        cards[i] = deck.dealCard();
        std::cout << cards[i] << ' ';
    }

    std::cout << '\n';

    std::cout << straightFlush(cards);


    return 0;
}