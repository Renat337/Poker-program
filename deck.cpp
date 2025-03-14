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
        static constexpr std::array ranks { 'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K' };
        static constexpr std::array suits { 'C', 'D', 'H', 'S' };

        out << ranks[card.rank] << suits[card.suit];
        return out;
    }

    bool operator==(Card otherCard)
    {
        return ((rank == otherCard.rank) && (suit == otherCard.suit));
    }

    inline static bool groupBySuitSort {true};

    bool operator<(const Card& otherCard) const
    {

        auto cardVal {(rank == rank_ace) ? max_ranks : rank};
        auto otherCardVal {(otherCard.rank == rank_ace) ? max_ranks : otherCard.rank};

        if (groupBySuitSort)
        {
            if (suit != otherCard.suit)
            {
                return suit < otherCard.suit;
            }

            return cardVal < otherCardVal;
        }
        
        if (cardVal != otherCardVal)
        {
            return cardVal < otherCardVal;
        }

        return suit < otherCard.suit;

    }
};

class Deck
{
    private:
        static constexpr size_t m_decksize {52};
        std::array<Card, m_decksize> m_deck {};
        std::size_t m_nextCardIndex {0};
        std::vector <Card> m_cardsChosen {};

    public:
        Deck()
        {
            size_t counter {0};
            for (auto s : Card::allSuits)
            {
                for (auto r : Card::allRanks)
                {
                    m_deck[counter] = Card {r, s};
                    ++counter;
                }
            }
        }
        
        void shuffle()
        {
            m_cardsChosen.clear();
            std::shuffle(m_deck.begin(), m_deck.end(), Random::mt);
            m_nextCardIndex = 0;
        }

        void shuffle(const std::vector<Card>& cardsChosen)
        {
            m_cardsChosen = cardsChosen;
            std::shuffle(m_deck.begin(), m_deck.end(), Random::mt);
            m_nextCardIndex = 0;
        }

        Card dealCard()
        {
            assert(m_nextCardIndex != 52 && "Deck::dealCard ran out of cards");
            while (in(m_deck[m_nextCardIndex],m_cardsChosen))
            {
                ++m_nextCardIndex;
            }
            return m_deck[m_nextCardIndex++];
        }

        bool in(Card card, const std::vector<Card>& set)
        {
            for (auto i : set)
            {
                if (card == i)
                {
                    return true;
                }
            }

            return false;
        }
};