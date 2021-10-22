// classCard.h: describing:  class CCard: base
//                           class CCardBJ : public CCard
//


#pragma once

#include <iostream>
#include <utility>

enum class CardRankT : char { ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGTH, NINE, TEN, JACK, QUEEN, KING } ;
enum class CardSuitT : char { CLUBS = 1, DIAMONDS, HEARTS, SPADES };

class CCard {						// representing a Card in General
	protected:
		CardRankT	m_rank ;
		CardSuitT	m_suit ;
		bool		m_isFU ;                   // If the face is up, ie open card

	public:				
		explicit CCard(const bool fl = true) ;          // generate a random card
		explicit CCard(const CardRankT& r, const CardSuitT& s, const bool fl = false) :     // constructor: DEFAULT
						m_rank(r), m_suit(s), m_isFU(fl) { /*cout << "\n__ CCard ..." ; */ }
		virtual ~CCard() { /*cout << "\n__ ~ CCard ..." ;*/ }

		CCard(const CCard& c)  ;				// Constructor: COPY 
		CCard(      CCard&& c) noexcept ;		// ...:         MOVE
		
		virtual CCard& operator = (const CCard& c) ;
		virtual CCard& operator = (      CCard&& c) noexcept ;

		void        flip()   { m_isFU = !m_isFU ; }		// Flips a card		  
		virtual int cardValue() const = 0 ;             // Abstract as the value depends on the game to be played

		friend std::ostream & operator <<(std::ostream& os, const CCard& card) ;
}; // class CCard

class CCardBJ : public CCard {		// representing a card in Black Jack
	public:
		explicit CCardBJ(const bool fl = true) : CCard(fl) { /*cout << "\n__ CCardBJ Def ..." ;*/ } // Default: generate a random card
		explicit CCardBJ(const CardRankT& r, const CardSuitT& s, const bool fl = false) :
		         CCard(r, s, fl) { /*cout << "\n__ CCardBJ ..." ;*/ }
		virtual ~CCardBJ() { /*cout << "\n__ ~ CCardBJ ..." ;*/ }

		CCardBJ(const CCardBJ& c) : CCard(c) { /*std::cout << "\n__ CCardBJ COPY (&) ..." ;*/ }
		CCardBJ(CCardBJ&& c) noexcept : CCard(std::move(c)) { /*std::cout << "\n__ CCardBJ MOVE (&&) ..." ;*/ }

		CCardBJ& operator =(const CCardBJ& c) ;
		CCardBJ& operator =(      CCardBJ && c) noexcept ;

		int   cardValue() const override ;
} ; // class CCardBJ


/* eof classCard.h */
