#include "deck.cpp"
#include <ranges>
#include <functional>

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

    [[maybe_unused]] static constexpr std::array<Rankings, max_rankings> allRankings {high_card, pair, two_pair, three_kind, straight,
                                                                    flush, full_house, four_kind, straight_flush};

    std::ostream& operator<<(std::ostream& out, const Rankings ranking)
    {
        static constexpr std::array rankings {"high card", "pair", "two pair", "three of a kind", "straight", "flush", 
                                                "full house", "four of a kind", "straight flush"};

        out << rankings[ranking];
        return out;
    }
}

struct Player
{
    std::array<Card, 2> hand {};
    int chips {Settings::buyIn};
    Settings::Rankings handType {};
    std::vector<Card> bestHand {};

    bool isPlayer {false};

    bool operator<(const Player otherPlayer) const
    {
        if (handType != otherPlayer.handType)
        {
            return handType < otherPlayer.handType;
        }

        for (std::size_t i {0}; i<5; ++i)
        {
            int value {(bestHand[i].rank == Card::rank_ace) ? Card::max_ranks : bestHand[i].rank};
            int otherPlayerValue {(otherPlayer.bestHand[i].rank == Card::rank_ace) ? Card::max_ranks : 
                otherPlayer.bestHand[i].rank};
            if (value != otherPlayerValue)
            {
                return value < otherPlayerValue;
            }
        }

        return false;
    }

    void print()
    {
        std::cout << hand.data()[0] << ' ' << hand.data()[1] << '\n';
        std::cout << handType << '\n';
        
        for (const auto& i : bestHand)
        {
            std::cout << i << ' ';
        }
        std::cout << '\n';
    }
};

template <typename T>
std::vector<Card> flush(const T& cards)
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
        if (!(i.rank == bestHand.back().rank))
        {
            bestHand.push_back(i);
            return bestHand;
        }
    }

    return {};
}

template <typename T>
std::vector<Card> threeOfKind(const T& cards)
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

        if (rankCount == 3)
        {
            break;
        }
    }

    if (rankCount < 3)
    {
        return {};
    }

    int cardsAdded {0};
    for (const auto& i : std::views::reverse(cardsCopy))
    {
        if (!(i.rank == bestHand.data()[0].rank))
        {
            bestHand.push_back(i);
            ++cardsAdded;
        }
        if (cardsAdded == 2)
        {
            return bestHand;
        }
    }

    return {};
}

template <typename T>
std::vector<Card> pair(const T& cards)
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

        if (rankCount == 2)
        {
            break;
        }
    }

    if (rankCount < 2)
    {
        return {};
    }

    int cardsAdded {0};
    for (const auto& i : std::views::reverse(cardsCopy))
    {
        if (!(i.rank == bestHand.data()[0].rank))
        {
            bestHand.push_back(i);
            ++cardsAdded;
        }
        if (cardsAdded == 3)
        {
            return bestHand;
        }
    }

    return {};
}

template <typename T>
std::vector<Card> fullHouse(const T& cards)
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

        if (rankCount == 3)
        {
            break;
        }
    }

    if (rankCount < 3)
    {
        return {};
    }

    Card::Ranks foundThree {bestHand.data()[0].rank};
    std::vector<Card> findTwo {};
    rankCount = 0;

    for (const auto& i : std::views::reverse(cardsCopy))
    {
        if (!findTwo.empty() && findTwo.back().rank == i.rank && !(foundThree == i.rank))
        {
            ++rankCount;
            findTwo.push_back(i);
        }
        else if (!(foundThree == i.rank))
        {
            findTwo.clear();
            findTwo.push_back(i);
            rankCount = 1;
        }
        else
        {
            rankCount = 0;
        }

        if (rankCount == 2)
        {
            break;
        }
    }

    if (rankCount < 2)
    {
        return {};
    }

    bestHand.push_back(findTwo.data()[0]);
    bestHand.push_back(findTwo.data()[1]);

    return bestHand;

}

template <typename T>
std::vector<Card> twoPair(const T& cards)
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

        if (rankCount == 2)
        {
            break;
        }
    }

    if (rankCount < 2)
    {
        return {};
    }

    Card::Ranks foundFirstTwo {bestHand.data()[0].rank};
    std::vector<Card> findTwo {};
    rankCount = 0;

    for (const auto& i : std::views::reverse(cardsCopy))
    {
        if (!findTwo.empty() && findTwo.back().rank == i.rank && !(foundFirstTwo == i.rank))
        {
            ++rankCount;
            findTwo.push_back(i);
        }
        else if (!(foundFirstTwo == i.rank))
        {
            findTwo.clear();
            findTwo.push_back(i);
            rankCount = 1;
        }
        else
        {
            rankCount = 0;
        }

        if (rankCount == 2)
        {
            break;
        }
    }

    if (rankCount < 2)
    {
        return {};
    }

    bestHand.push_back(findTwo.data()[0]);
    bestHand.push_back(findTwo.data()[1]);

    Card::Ranks foundSecondTwo {bestHand.back().rank};

    for (const auto& i : std::views::reverse(cardsCopy))
    {
        if (!(i.rank == foundFirstTwo || i.rank == foundSecondTwo))
        {
            bestHand.push_back(i);
            return bestHand;
        }
    }

    return {};
}

template <typename T>
std::vector<Card> highCard(const T& cards)
{
    std::vector<Card> bestHand {};
    bestHand.reserve(5);
    T cardsCopy = cards;

    Card::groupBySuitSort = false;
    std::sort(cardsCopy.begin(),cardsCopy.end());

    for (const auto& i : std::views::reverse(cardsCopy))
    {
        bestHand.push_back(i);
    }

    return bestHand;
}

struct HandFunction
{
    using HandEvaluator = std::function<std::vector<Card>(std::array<Card,7>)>;
    
    HandEvaluator function;
    Settings::Rankings ranking;
};

int playGame(int numPlayers, std::array<Card, 2>& hand)
{
    Deck deck {};
    deck.shuffle();

    std::array<Card,7> communalCards {};

    std::vector<Player> players(static_cast<std::size_t>(numPlayers));
    players.data()[0].isPlayer = true;
    players.data()[0].hand = hand;

    for (auto& i : players)
    {
        if (!i.isPlayer)
        {
            i.hand.data()[0] = deck.dealCard();
            i.hand.data()[1] = deck.dealCard();
        }
        // std::cout << i.hand.data()[0] << ' ' << i.hand.data()[1] << '\n';
    }

    // std::cout << '\n';

    // betting here

    for (std::size_t i {0}; i<3; ++i)
    {
        communalCards[i] = deck.dealCard();
        // std::cout << communalCards[i] << ' ';
    }

    // std::cout << '\n';

    // betting here

    communalCards.data()[3] = deck.dealCard();
    // std::cout << communalCards.data()[3] << '\n';

    //betting here

    communalCards.data()[4] = deck.dealCard();
    // std::cout << communalCards.data()[4] << '\n';

    //betting here

    const std::vector<HandFunction> handFunctions {
        {&straightFlush<std::array<Card,7>>, Settings::straight_flush},
        {&fourOfKind<std::array<Card,7>>, Settings::four_kind},
        {&fullHouse<std::array<Card,7>>, Settings::full_house},
        {&flush<std::array<Card,7>>, Settings::flush},
        {&straight<std::array<Card,7>>, Settings::straight},
        {&threeOfKind<std::array<Card,7>>, Settings::three_kind},
        {&twoPair<std::array<Card,7>>, Settings::two_pair},
        {&pair<std::array<Card,7>>, Settings::pair},
        {&highCard<std::array<Card,7>>, Settings::high_card}
    };

    Settings::Rankings bestHandType {Settings::high_card};
    std::vector<Player> winning {};

    for (auto& player : players)
    {
        communalCards[5] = player.hand[0];
        communalCards[6] = player.hand[1];

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

    // need to implement draw

    std::sort(winning.begin(), winning.end());

    if (std::ssize(winning) > 1)
    {
        Player final = winning.back();
        winning.pop_back();
        Player penultimate = winning.back();

        if (final < penultimate && penultimate < final)
        {
            std::cout << "Draw!!!" << '\n';
            return 0;
        }

        return final.isPlayer;
    }

    return winning.back().isPlayer;

}

int main()
{
    int numPlayers {2};
    int tries {100000};
    int wins {0};

    std::array<Card, 2> hand {{{Card::rank_ace,Card::suit_clubs},{Card::rank_ace,Card::suit_diamonds}}};
    
    for (int i {0}; i<tries; ++i)
    {
        if (playGame(numPlayers, hand))
        {
            ++wins;
        }
    }

    std::cout << static_cast<double>(wins)/tries << '\n';

    return 0;
}