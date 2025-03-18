#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>
#include <vector>
#include "Random.h"
#include "deck.h"


bool Card::operator==(Card otherCard)
{
    return ((rank == otherCard.rank) && (suit == otherCard.suit));
}

bool Card::operator<(const Card& otherCard) const
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

Deck::Deck()
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

void Deck::shuffle()
{
    m_cardsChosen.clear();
    std::shuffle(m_deck.begin(), m_deck.end(), Random::mt);
    m_nextCardIndex = 0;
}

void Deck::shuffle(const std::vector<Card>& cardsChosen)
{
    m_cardsChosen = cardsChosen;
    std::shuffle(m_deck.begin(), m_deck.end(), Random::mt);
    m_nextCardIndex = 0;
}

Card Deck::dealCard()
{
    assert(m_nextCardIndex != 52 && "Deck::dealCard ran out of cards");
    while (in(m_deck[m_nextCardIndex],m_cardsChosen))
    {
        ++m_nextCardIndex;
    }
    return m_deck[m_nextCardIndex++];
}

bool Deck::in(Card card, const std::vector<Card>& set)
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