// classCard.cpp: member functions of classes CCard and CCardBJ follow
//


#include "classCard.h"


CCard::CCard(const bool fl)
{ 
						static unsigned int iVal = static_cast<unsigned int>(time(0)) ;
														// std::cout << "\n__ CCard Default ..." ;
	srand(iVal) , iVal += 100 ;
	
	m_rank = static_cast<CardRankT>(rand() % static_cast<unsigned int>(CardRankT::KING) + 1),
		m_suit = static_cast<CardSuitT>(rand() % static_cast<unsigned int>(CardSuitT::SPADES) + 1),
		m_isFU = fl ;
} // CCard::CCard()

CCard::CCard(const CCard& card)
{
														// std::cout << "\n__ CCard COPY " << card << " ...";
	m_rank = card.m_rank, m_suit = card.m_suit, m_isFU = card.m_isFU ;
} // CCard::CCard(&)

CCard::CCard(CCard&& c) noexcept
{
														// std::cout << "\n__ CCard MOVE " << c << " ..." ;
	m_rank = c.m_rank, m_suit = c.m_suit, m_isFU = c.m_isFU ;
}

CCard&
CCard::operator =(const CCard& c)
{
														// std::cout << "\n__ CCard = (& " << c << ") ..." ;
	if (this != &c) {
		m_rank = c.m_rank, m_suit = c.m_suit, m_isFU = c.m_isFU ;
	}
	return(*this) ;
} // CCard::operator =(&)

CCard&
CCard::operator = (CCard&& c) noexcept
{
														// std::cout << "\n__ CCard = (&& " << c << ") ..." ;
	if (this != &c) {
		m_rank = c.m_rank, m_suit = c.m_suit, m_isFU = c.m_isFU ;
	}
	return(*this) ;
}

std::ostream&
operator <<(std::ostream& os, const CCard& card)
{
	const char * cardR[] = { "X", "A", "2", "3", "4", "5", "6", "7", "8", "9", "T", "J", "Q", "K" } ;
	const char * cardS[] = { "X", "Cl", "Di", "He", "Sp" } ;

	size_t		iR = static_cast<size_t>(card.m_rank) ;
	size_t      iS = static_cast<size_t>(card.m_suit) ;

	if (!(card.m_isFU))          iR = iS = 0 ;
	os << "{" << cardR[iR] << ":" << cardS[iS] << '}' ;
	return(os) ;
} // friend CCard operator <<()


// Member functions for class CCardBJ follow

int
CCardBJ::cardValue() const			// The VALUE of a card: if !m_isFU - 0
{
	int   value = 0 ;

	if (m_isFU) {
		value = static_cast<int>((m_rank > CardRankT::NINE) ? CardRankT::TEN : m_rank) ;
	}
	return(value) ;
} // CCardBJ::cardValue()


CCardBJ&
CCardBJ::operator =(const CCardBJ& c)
{
														// std::cout << "\n__ CCardBJ = (& " << c << ") ..." ;
	if (this != &c) {
		m_rank = c.m_rank, m_suit = c.m_suit, m_isFU = c.m_isFU ;
	}
	return(*this) ;
} // CCardBJ operator = (&)

CCardBJ&
CCardBJ::operator =(CCardBJ&& c) noexcept
{
														// std::cout << "\n__ CCardBJ = (&& " << c << ") ..." ;
	if (this != &c) {
		m_rank = c.m_rank, m_suit = c.m_suit, m_isFU = c.m_isFU ;
	}
	return(*this) ;
} // CCardBJ operator = (&)

/* eof classCard.cpp */