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

template <typename T>
std::vector<Card> flush(const T& cards)
// vector output as it is moveable
{
    std::vector<Card> bestHand(5);

    Card::groupBySuitSort = true;
    T cardsCopy = cards;
    std::sort(cardsCopy.begin(),cardsCopy.end());
    
    Card::Suits prevSuit {Card::max_suits};
    std::size_t curPos {};
    for (const auto& i : std::views::reverse(cardsCopy))
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

template <typename T>
std::vector<Card> straight(const T& cards)
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

template <typename T>
std::vector<Card> straightFlush(const T& cards)
{
    std::array suitCounter {0,0,0,0};
    Card::Suits maxSuit  {Card::max_suits};
    int maxSuitCount {0};

    for (const auto& i : cards)
    {
        if ((++suitCounter.data()[i.suit]) > maxSuitCount)
        {
            maxSuitCount = suitCounter.data()[i.suit];
            maxSuit = i.suit;
        }
    }

    if (maxSuitCount < 5)
    {
        return {};
    }

    std::vector<Card> candidates {};
    candidates.reserve(std::size(cards));

    for (const auto& i : cards)
    {
        if (i.suit == maxSuit)
        {
            candidates.push_back(i);
        }
    }

    return straight(candidates);
}

template <typename T>
std::vector<Card> fourOfKind(const T& cards)
{
    std::vector<Card> bestHand {};
    bestHand.reserve(5);
    T cardsCopy = cards;

    Card::groupBySuitSort = false;
    std::sort(cardsCopy.begin(),cardsCopy.end());

    int rankCount {0};
    for (const auto& i : std::views::reverse(cardsCopy))
    {
        if (!bestHand.empty() && bestHand.back().rank == i.rank)
        {
            ++rankCount;
            bestHand.push_back(i);
        }
        else
        {
            bestHand.clear();
            bestHand.push_back(i);
            rankCount = 1;
        }

        if (rankCount == 4)
        {
            break;
        }
    }

    if (rankCount < 4)
    {
        return {};
    }

    for (const auto& i : std::views::reverse(cardsCopy))
    {
        if (!(i == bestHand.back()))
        {
            bestHand.push_back(i);
            return bestHand;
        }
    }

    return {};
}

int main()
{
    std::vector<Card> testFourOfKind {
        Card{Card::rank_king, Card::suit_hearts},
        Card{Card::rank_king, Card::suit_spades},
        Card{Card::rank_king, Card::suit_diamonds},
        Card{Card::rank_king, Card::suit_clubs},
        Card{Card::rank_ace, Card::suit_spades},
        Card{Card::rank_3, Card::suit_hearts},
        Card{Card::rank_2, Card::suit_clubs}
    };

    Deck deck {};
    deck.shuffle(testFourOfKind);

    constexpr int drawnCards {0};
    std::array<Card,drawnCards> cards {};

    for (int i {0}; i<drawnCards; ++i)
    {
        cards.data()[i] = deck.dealCard();
        std::cout << cards.data()[i] << ' ';
    }

    std::cout << '\n';

    std::vector<Card> bestHand = fourOfKind(testFourOfKind);

    for (auto i : bestHand)
    {
        std::cout << i << ' ';
    }

    return 0;
}