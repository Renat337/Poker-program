#include "deck.h"
#include <ranges>
#include <functional>
#include <utility>

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

    Player(Card card1, Card card2)
    : hand {card1, card2} {}

    Player()
    {}

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
                // not sure why error was here
                // std::cout << "ERROR AT " << i<< '\n';
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
        std::cout << "Is player: " << isPlayer;
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

struct HandFunction
{
    using HandEvaluator = std::function<std::vector<Card>(std::vector<Card>)>;

    HandEvaluator function;
    Settings::Rankings ranking;
    
};

Settings::GameStates checkWinner(std::vector<Player>& players, std::vector<Card>& communalCards)
{
    static const std::vector<HandFunction> handFunctions
    {
    {&straightFlush<std::vector<Card>>, Settings::straight_flush},
    {&fourOfKind<std::vector<Card>>, Settings::four_kind},
    {&fullHouse<std::vector<Card>>, Settings::full_house},
    {&flush<std::vector<Card>>, Settings::flush},
    {&straight<std::vector<Card>>, Settings::straight},
    {&threeOfKind<std::vector<Card>>, Settings::three_kind},
    {&twoPair<std::vector<Card>>, Settings::two_pair},
    {&pair<std::vector<Card>>, Settings::pair},
    {&highCard<std::vector<Card>>, Settings::high_card}
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

void runGame(int tries)
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
    runGame(100000);

    return 0;
}