#pragma once

#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>
#include <vector>
#include "Random.h"

struct Card
{
    enum Suits
    {
        suit_clubs,
        suit_diamonds,
        suit_hearts,
        suit_spades,

        max_suits
    };

    enum Ranks
    {
        rank_ace,
        rank_2,
        rank_3,
        rank_4,
        rank_5,
        rank_6,
        rank_7,
        rank_8,
        rank_9,
        rank_10,
        rank_jack,
        rank_queen,
        rank_king,

        max_ranks
    };

    static constexpr std::array allRanks {rank_ace, rank_2, rank_3, rank_4, rank_5, rank_6, rank_7, rank_8,
                                            rank_9, rank_10, rank_jack, rank_queen, rank_king};
    static constexpr std::array allSuits {suit_clubs, suit_diamonds, suit_hearts, suit_spades};

    Ranks rank {};
    Suits suit {};

    friend std::ostream& operator<<(std::ostream& out, const Card& card)
    {
        out << ranks[card.rank] << suits[card.suit];
        return out;
    }

    bool operator==(Card otherCard);

    inline static bool groupBySuitSort {true};

    bool operator<(const Card& otherCard) const;

    friend std::istream& operator>>(std::istream& in, Card& card)
    {
        char r {};
        char s {};

        while (true)
        {
            in >> r >> s;

            if (in.fail())
            {
                in.clear();
                in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Try again: \n";
                continue;
            }

            auto posRank {std::find(card.ranks.begin(),card.ranks.end(),r)};
            auto posSuit {std::find(card.suits.begin(),card.suits.end(),s)};
            
            if (posRank == card.ranks.end() || posSuit == card.suits.end())
            {
                std::cout << "Invalid input. Try again: \n";
                continue;
            }
            
            card.rank = static_cast<Ranks>(std::distance(card.ranks.begin(), posRank));
            card.suit = static_cast<Suits>(std::distance(card.suits.begin(), posSuit));
    
            return in;
            
        }
    }

    private:
        static constexpr std::array ranks { 'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K' };
        static constexpr std::array suits { 'C', 'D', 'H', 'S' };
};

class Deck
{
    private:
        static constexpr size_t m_decksize {52};
        std::array<Card, m_decksize> m_deck {};
        std::size_t m_nextCardIndex {0};
        std::vector <Card> m_cardsChosen {};

    public:
        Deck();
        void shuffle();
        void shuffle(const std::vector<Card>& cardsChosen);
        Card dealCard();
        bool in(Card card, const std::vector<Card>& set);
};