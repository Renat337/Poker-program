#include "deck.cpp"

int value(const Card& card)
{
    static constexpr std::array rankValues {11, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};
    return rankValues[static_cast<std::size_t>(card.rank)];
}

struct Player
{
    Card lastcard {};
    int score {0};
    std::vector<Card> hand {};
    int numaces {0};

    void drawCard(Deck& deck)
    {
        lastcard = deck.dealCard();
        score += value(lastcard);
        hand.push_back(lastcard);

        if (lastcard.rank == Card::rank_ace) {++numaces;};
    }
};

namespace Settings
{
    const int bust {21};
    const int dealerStop {17};
}

bool playerTurn(Player& player, Deck& deck)
{
    std::cout << "Would you like to hit or stay? 1 for hit, 0 for stay: \n";
    int hit {};
    std::cin >> hit;

    while (player.score <= Settings::bust && hit)
    {
        player.drawCard(deck);

        while (player.score > Settings::bust && player.numaces > 0)
        {
            player.score -= 10;
            --player.numaces;
        }

        std::cout << "Player drew: " << player.lastcard << ". Player score: ";

        if (!player.numaces)
        {
            std::cout << ' ' << player.score;
        }
        else
        {
            std::string sep {""};
            for (int i {player.numaces}; i>=0; --i)
            {
                std::cout << sep << player.score - 10*i;
                sep = " or ";
            }
        }

        std::cout << '\n';

        if (player.score > Settings::bust)
        {
            std::cout << "Player bust! Dealer wins!\n";
            return false;
        }

        std::cout << "Hit or stay?\n";
        std::cin >> hit;
    }

    return true;
}

bool dealerTurn(Player& dealer, Deck& deck)
{
    while (dealer.score<Settings::dealerStop)
    {
        dealer.drawCard(deck);

        while (dealer.score > Settings::bust && dealer.numaces > 0)
        {
            dealer.score -= 10;
            --dealer.numaces;
        }

        std::cout << "Dealer drew: " << dealer.lastcard << ". Dealer score: ";

        if (!dealer.numaces)
        {
            std::cout << ' ' << dealer.score;
        }
        else
        {
            std::string sep {""};
            for (int i {dealer.numaces}; i>=0; --i)
            {
                std::cout << sep << dealer.score - 10*i;
                sep = " or ";
            }
        }

        std::cout << '\n';
    }

    if (dealer.score > Settings::bust)
    {
        std::cout << "Dealer bust! Player wins!\n";
        return false;
    }
    
    return true;
}

int playBlackjack(Deck& deck)
{
    Player dealer;
    dealer.drawCard(deck);
    std::cout << "Dealer is showing: " << dealer.lastcard << ". Their score is: " << dealer.score << '\n';

    Player player;
    player.drawCard(deck);
    player.drawCard(deck);

    std::cout << "Player is showing: " << player.hand[0] << ' ' << player.hand[1]
              << ". Your score is: " << player.score << '\n';

    if (!playerTurn(player,deck))
    {
        return 0;
    }
    
    if (!dealerTurn(dealer,deck))
    {
        return 0;
    }

    if (player.score > dealer.score)
    {
        std::cout << "Player wins!\n";
    }
    else if (player.score < dealer.score)
    {
        std::cout << "Dealer wins!\n";
    }
    else
    {
        std::cout << "It is a draw.\n";
    }

    return 0;

}

int main()
{
    Deck deck {};
    deck.shuffle();

    playBlackjack(deck);

    return 0;
}