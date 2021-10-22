
// classHand.h: describes a hand
//

#pragma once

#include <iostream>
#include <vector>
#include <algorithm>

#include "llib\vArray.h"
#include "classCard.h"


class CHand			 {
	friend ostream& operator << (ostream& os, CHand& hand) ;

	protected:
			varray<CCardBJ *> m_store ;

	public:
		explicit CHand(const size_t max = 0) : m_store(0, nullptr, max) { /*cout << "\n__ CHand 0 of " << max << " cards ..." ;*/ }
		virtual ~CHand() { /*cout << "\n__ ~ CHand ..." ;*/ this->clear() ; }

		CHand(const CHand&) ;                        // Copy ..
		CHand& operator =(const CHand&) ;            // ...

	public:
		virtual bool	isEmpty() const		{ return(m_store.size() == 0) ; }
		size_t	size() const				{ return(m_store.size()) ; }

		bool	receive(CCardBJ *pCard) ;		// the hand receives a BJ card pointed to by 'pCard'. Store the pointer 
		void	clear() ;						// Clear the hand (no cards in it) empty

		int		getTotal() const ;				// Calculates a hand's value
}; // class CHand

class CDeck : public CHand {    // Represents a deck of cards
	public:
		explicit	CDeck(const size_t cap = 52) : CHand(cap) { populate() , shuffle() ; }
		virtual		~CDeck() { /*cout << "\n__ ~ CDeck ..." ; */ }

		bool populate() ;   // Generate a new Deck of cards
		void shuffle() ;    // Shuffle the deck 
		virtual bool	isEmpty() ;

		bool	deal(CHand& pPlayer) ; // Deal a card to a hand(or a Player)
}; // class CDeck

/* eof classHand.h */