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

    enum GameStates
    {
        loss,
        draw,
        win,

        max_gamestates
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

    bool operator==(const Player otherPlayer) const
    {
        if (handType != otherPlayer.handType)
        {
            return false;
        }

        for (std::size_t i {0}; i<5; ++i)
        {
            if (bestHand[i].rank != otherPlayer.bestHand[i].rank)
            {
                std::cout << "ERROR AT " << i<< '\n';
                return false;
            }
        }

        return true;
    }

    void print() const
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

std::vector<Player> dealHands(Deck& deck, std::size_t numPlayers, bool controlHands)
{
    std::vector<Player> players(numPlayers);

    if (controlHands)
    {
        for (std::size_t i {0}; i<numPlayers; ++i)
        {
            std::cout << "Input the hand for player" << i << '\n';
            std::cin >> players[i].hand.data()[0] >> players[i].hand.data()[1];
        }
        return players;
    }

    deck.shuffle();

    for (auto& i : players)
    {
        i.hand.data()[0] = deck.dealCard();
        i.hand.data()[1] = deck.dealCard();
    }

    return players;
    
}

 std::vector<Player> gameBeginning(std::array<Card,7>& communalCards)
 {
    Deck deck {};

    std::size_t numPlayers {};
    std::cout << "Number of players:\n";
    std::cin >> numPlayers;

    std::cout << "Type i to input their hands, otherwise type n: ";
    char controlHandsChar {};
    std::cin >> controlHandsChar;
    bool controlHands {controlHandsChar == 'i'};

    std::vector<Player> players = dealHands(deck, numPlayers, controlHands);
    std::vector<Card> usedCards {};
    usedCards.reserve(5+2*numPlayers);
    players.data()[0].isPlayer = true;

    char controlCommunalChar {};
    bool controlCommunal {};

    if (controlHands)
    {
        std::cout << "Do you want to control the communal cards as well? Type i or n:";
        std::cin >> controlCommunalChar;
        controlCommunal = controlCommunalChar == 'i';
        
        for (const auto& i : players)
        {
            usedCards.push_back(i.hand.data()[0]);
            usedCards.push_back(i.hand.data()[1]);
        }

        if (controlCommunal)
        {
            std::cout << "How many communal cards would you like to control?";
            std::size_t numControl {};
            std::cin >> numControl;

            assert(numControl <= 5);

            for (std::size_t i {0}; i<numControl; ++i)
            {
                std::cout << "Input communal card " << i;
                std::cin >> communalCards[i];
                usedCards.push_back(communalCards[i]);
            }

            deck.shuffle(usedCards);

            for (std::size_t i{numControl}; i<5; ++i)
            {
                communalCards[i] = deck.dealCard();
            }
        }
        else
        {
            deck.shuffle(usedCards);
            for (std::size_t i {0}; i<5; ++i)
            {
                communalCards[i] = deck.dealCard();
                // std::cout << communalCards[i] << ' ';
            }
        }
    }
    else
    {
        deck.shuffle();

        for (std::size_t i {0}; i<5; ++i)
        {
            communalCards[i] = deck.dealCard();
            // std::cout << communalCards[i] << ' ';
        }

    }

    return players;
 }

Settings::GameStates checkWinner(std::vector<Player>& players, std::array<Card, 7>& communalCards)
{
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

int main()
{
    int tries {1};
    int wins {0};
    int draws {0};

    Deck deck {};
    std::array<Card, 7> communalCards {};
    std::vector<Player> players {gameBeginning(communalCards)};

    for (auto& i : communalCards)
    {
        std::cout << i << ' ';
    }
    std::cout << '\n';
    
    for (int i {0}; i<tries; ++i)
    {
        Settings::GameStates game {checkWinner(players, communalCards)};

        for (auto& i : players)
        {
            i.print();
        }
        std::cout << '\n';

        if (game == Settings::win)
        {
            ++wins;
        }
        if (game == Settings::draw)
        {
            ++draws;
        }
    }

    std::cout << '\n' << wins << ' ' << draws << ' ' << tries - draws - wins << '\n';

    return 0;
}