// classHand.cpp: member functions, etc of CHand, CDeck classes
//

#include "llib\ownMenu.h"
#include "classHand.h"


// CHand member functions, etc follow

CHand::CHand(const CHand& hand)
{
																// cout << "\n__ CCHand Copy {" << (hand.m_store).size() << " of " 
																// << (hand.m_store).capacity() << "} ..." ;
	m_store = hand.m_store ;
} // CHand ()

CHand&
CHand::operator =(const CHand & hand)
{
																// cout << "\n__ CHand =() {" << (hand.m_store).size() << " of " 
																// << (hand.m_store).capacity() << "} ..." ;
	if (this != &hand) {
		m_store = hand.m_store ;
	}
	return(*this) ;
} // CHand operator =()


std::ostream&
operator <<(std::ostream& os, CHand& hand)
{
	size_t	numC = (hand.m_store).size() ;
	for (size_t i = 0 ; i < numC ; i++) {
		if (i % 13 == 0 && i > 0)       cout << endl ;
		cout << *((hand.m_store)[i]) << ' ' ;
	}
	return(os) ;
} // friend CHand operator << ()

bool
CHand::receive(CCardBJ * pCard)
{
	return(m_store.push_back(pCard)) ;
} // CHand::receive()

int
CHand::getTotal() const
{
	int      total = 0, val = 0 ;
	size_t   nAces = 0 ;

	if (isEmpty())     return(0) ;

	for (auto pC = begin(m_store) ; pC != end(m_store) ; ++pC)  {	// OLDOLD size_t i = 0 ; i < m_store.size() ; i++) {
		val = (*pC)->cardValue() ,									// POINTERS_POINTERS
		total += val ;    // the value of a closed card will be 0
		if (val == static_cast<int>(CardRankT::ACE))    nAces++ ;
	}
	// Adjust for Ace = 1/11
	if (total <= 11 && nAces > 0)      total += 10 ;   // Might adjust for one Ace only
	return(total) ;
} // CHand::getTotal()

void
CHand::clear()          // clears 'm_store' of contents, ie to EMPTY = no cards in    // POINTERS_POINTERS
{
	// Free what's pointed to
	for (size_t i = 0 ; i < m_store.size() ; i++) {
		if ((m_store[i]) != nullptr)       delete (m_store[i]), (m_store[i]) = nullptr ;
	}
	m_store.clear() ;    // = no elements stored
} // CHand::clear() 
// eol CHand

// CDeck member functions, etc follow

bool
CDeck::populate()								// POINTERS_POINTERS
{
	cout << "\n-- new Deck," ; // new Deck ... " ;
	clear() ;    // the Hand
	for (int j = static_cast<int>(CardSuitT::CLUBS) ; j <= static_cast<int>(CardSuitT::SPADES) ; j++) {
		for (int i = static_cast<int>(CardRankT::ACE) ; i <= static_cast<int>(CardRankT::KING) ; i++) {
			try {
				receive(new CCardBJ(static_cast<CardRankT>(i), static_cast<CardSuitT>(j), true)) ;
			} catch (std::bad_alloc) { cout << endl << "__ Memory allocation failure.Exiting ...\n" ; exit(1) ; }
		}
		// Probably a BETTER care of memory allocation must be taken but ...
	}
	return(true) ;
} // CDeck::populate()

void
CDeck::shuffle()								// POINTERS_POINTERS
{
	std::vector <CCardBJ *>		wdeck ;
	size_t						sizeDeck = m_store.size() ;

	cout << "\n-- the Deck is being shuffled ..." ;
	wdeck.reserve(52) ;
	srand(static_cast<unsigned int>(time(0))) ;
	// Transfer * cards to the vector for shuffling
												// cout << endl << "___ the Deck is: " << m_store ;
	for (size_t i = 0 ; i < sizeDeck ; i++)		wdeck.push_back(m_store[i]) ;
	random_shuffle(begin(wdeck), end(wdeck)) ;
	
	// Transfer back ...
	sizeDeck = 0 ;
	for (auto iter = cbegin(wdeck) ; iter != cend(wdeck) ; ++iter) {
		m_store[sizeDeck++] = *iter ;
	}
	cout << " Done." ;
} // Cdeck::shuffle()

bool
CDeck::isEmpty()
{
	const size_t	limit = 10 ;
	size_t			n_cards = m_store.size() ;
	static bool     fl_pop = true ;

	if (n_cards > 0 && n_cards <= limit && fl_pop) {
		CMenu  menu("-- the Deck is running low - ", "repopulate ? y)es or n)o ") ;
		cout << menu ;
		if (menu.choice(false) == 0) {				// Ask to Repopulate
			populate(), shuffle() ;
			fl_pop = true ;
		} else fl_pop = false ;
	} else if (n_cards == 0) {						// Must repopulate.
		CMenu  menu("-- the Deck is OUT of cards: Repopulating ...", "-") ;
		cout << menu ;
		populate(), shuffle() ; fl_pop = true ;
	}
	return(false) ;
} // CDeck::isEmpty()

bool
CDeck::deal(CHand& hand)						// POINTERS_POINTERS
{
	if (isEmpty()) {
		cout << endl << "__ Out of cards: Should have NOT happened. Exiting..." ;
		return(false) ;
	}
														// cout << endl << "__ dealing a card to - " << hand ;
	// Now the deck is not empty - the .pop() should be OK
	CCardBJ * pCard = (this->m_store).back() ; (this->m_store).pop_back() ;
	bool	  fl = false ;
	if (!(fl = hand.receive(pCard)))      m_store.push_back(pCard) ;  // 'hand' must be full -> unDeal ;
	return(fl) ;
} // CDeck::deal()

// eol CDeck

// eof classHand.cpp