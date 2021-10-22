// game_BJ.cpp: the game of Black Jack, idea of M.Dawson
//              but through (and to test) ownArray(s) and class CMenu
//    

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

#include "llib\ownMenu.h"
#include "llib\ownCIN.h"

#include "classCard.h"
#include "classHand.h"

#include "game_BJ.h"


int
main()
{
						const size_t	maxNumberP = MAX_NUM_PLAYERS ;         // Number of Players allowed
						size_t			mCh = 0 ;
						CMenu	menu("> Table of Black Jack", 
									 string("  (1) Play   (2) Show Table  (3) Manage Seats\n  (0) Exit ...")) ;
						
						CTableBJ	table ;

	do {
		cout << endl << endl << menu << endl << endl ;
		if ((mCh = menu.choice()) == 1)     {
			table.showTable() ;												// to test: table.showPlayers() ;
		} else if (mCh == 2)      {
								CMenu	menuS("> Manage Seats at the Table - ", string("(1) Take a seat   (2) Leave   ") +
																   string("(3) Invite Players\n(0) Exit...")) ;
								size_t  ch ;
			do {
				cout << endl << menuS ;
				if ((ch = menuS.choice()) == 0) {
					if (table.nVacant() > 0)			table.addPlayer() ;
					else								cout << endl << endl << "-- No Vacant seat at the Table..." ;
				} else if (ch == 1)   {
					if (table.numPlayers() > 0)     table.removePlayer() ;
					else							cout << endl << endl << "-- No players at the Table..." ;
				} else if (ch == 2) {
					if (table.nVacant() > 0)		table.addRPlayers() ;
					else							cout << endl << endl << "-- No Vacant seat at the Table..." ;
				}
			} while (ch != 3) ;
		} else if (mCh == 0)                {
			if (table.numPlayers() > 0)		table.play() ;
			else							cout << endl << endl << "-- No players at the Table..." ;
		}

		cout << endl ;
	} while (mCh != 3) ;
	
	return(0) ;
} // main()


bool
CTableBJ::addPlayer()
{
						size_t		seat = chooseSeat(true) ;  // The actul # in [1:...] or '0'  if none
						char		buff[MaxNameLen] ;				

	if (seat == 0)          return(false) ;   // No seat has been chosen
	seat-- ;

	inp_Txt(buff, sizeof(buff), ">> Your name, please ? ") ;
	if (buff[0] == 0)			strcpy_s(buff, sizeof(buff), "<NO NAME>") ;

	_addPlayer(&(m_aPlayers[seat]), string(buff)) ;

	cout << endl << "-- please, Welcome <" << buff << "> at seat " << seat + 1 ;
	cout << endl << "-- " << m_nP << " player" << (m_nP > 1 ? "s" : "") << " at the table !\n" ;
	return(true) ;
} // CTableBJ::addPlayer()

string
genName(size_t i)
{
	std::stringstream	ss ;
	ss << "Gambler " << i ;
	return(ss.str()) ;
} // genName()

bool
CTableBJ::addRPlayers()
{
	// Define 'random' # of players to enter (srand() launched before shuffling )
	// if ((numRP = rand() % nVacant()) == 0)     numRP = 1 ;
	size_t	numRP = 0 ;
	size_t  nSeats = m_aPlayers.capacity() ;
	for (size_t i = 0 ; i < nSeats ; i++)    {
		if (isFree(i) && (rand() % 3 == 0)) {
			_addPlayer(&(m_aPlayers[i]), std::move(genName(i))) ;
			numRP ++ ;
		}
	}
	if (numRP == 0)   { cout << endl << "-- No Players available: try again later, please ..." ; return(false) ; }

	return(true) ;
} // CTableBJ::addRPlayers()

void
CTableBJ::_addPlayer(CPlayer *pPlayer, string&& str)
{
																	// cout << endl << "__ <" << str << "> enters ..." ;
	*pPlayer = std::move(string(str)) ; pPlayer->clear() ; // clear the hand - just in case
	m_nP++ ;
} // CTable::_addPlayer()

bool
CTableBJ::removePlayer()
{
						size_t		seat = chooseSeat(false) ;  // The actul # in [1:...] or '0'  if none

	if (seat == 0)          return(false) ;   // No seat has been chosen
	seat-- ;

	CPlayer   * pPlayer = &(m_aPlayers[seat]) ;
	cout << endl << endl << "-- " , pPlayer->showPlayer() , cout << " is to leave the Table" ;

	CMenu mConf("  :  ", "C)onfirm or N)o") ;
	cout << mConf ;
	if (mConf.choice(true, false) == 1)        return(false) ;   // removal stopped

	cout << endl << "-- "; pPlayer->showPlayer(), cout << " has left the Table ..." ;
	if (m_nP > 0)          m_nP-- ;
	*pPlayer = string("") , pPlayer->clear() ;  // Clear the hand - just in case
	
	cout << endl << "-- " << m_nP << " player" << (m_nP > 1 ? "s" : "") << " at the table !" ;
	return(true) ;
} // CTableBJ::removePlayer()

size_t
CTableBJ::chooseSeat(bool fl_vac) const         // to choose one of the vacant/occupied (as per fl_vac') seats
{
						const size_t	ILEN = 4 ;     // ILEN chars per seat
						size_t			nSeat = m_aPlayers.capacity() ;
						const size_t	buffSize = nSeat * ILEN ;

						const char  * const pT1 = "-- the Available seats are: " ;
						const char  * const pT2 = "-- the Occupied seats are: " ;
	
	char  *	const	constBuff = new char[buffSize + 1] ;
	if (constBuff == nullptr)     { cout << endl << "__ Memory allocation failure.Exiting ...\n" ; exit(1) ; }
	constBuff[0] = 0 ;

	char  * ptr = constBuff ;
	size_t  numFree = 0 ;
	for (size_t i = 0 ; i < nSeat ; i++) {
		if (fl_vac ? isFree(i) : (!isFree(i)))  	{
			//       *ptr = '(' , *(ptr + 1) = '1' + n , *(ptr + 2) = ')' , *(ptr + 3) = ' ' ;
			sprintf_s(ptr, ILEN + 1, "(%u) ", i + 1) ,
			numFree++ ,
			ptr += ILEN ;
		}
	}

	showTable() ;
	CMenu	seats((fl_vac ? pT1 : pT2), string(constBuff) + string("\n(0) Exit...")) ;
	cout << endl << endl << seats ;

	if ((nSeat = seats.choice()) < numFree)   {				// 'numFree' shows the Exit choice
		// Turn the menu choice in 'nSeat' into a seat # 
		nSeat = atoi(constBuff + nSeat * ILEN + 1) ;		// Just after the opening '('
	} else  nSeat = 0 ;     // = NO CHOICE 
	delete [] constBuff ;

	return(nSeat) ;
} // CTableBJ::chooseSeat()


bool
CTableBJ::isFree(size_t seat) const  // if 'seat' is vacant: counts [1:m_aPlayers.capacity()]
{
	// if (seat < m_aPlayers.capacity()) return(*((m_aPlayers[seat]).nameP()) == "") ;
	if (seat < m_aPlayers.capacity())  	 return(*((cbegin(m_aPlayers) + seat)->nameP()) == string("")) ;
	return(false) ;
} // CTableBJ::isFree()


void
CTableBJ::showTable() const
{
																					// CHand * pHand = &m_deck ;
	const string * pStr = nullptr ;

	cout << endl << "Table of Black Jack: " << (m_deck.size() < 52 ? "" : "NEW")
		<< " deck of " << m_deck.size() << " cards" ;
	cout << endl << "------------------------------------------" ;

	for (size_t i = 0, num = m_aPlayers.capacity() ; i < num ; i++) {
		pStr = (cbegin(m_aPlayers) + i)->nameP() ;
		cout << "\n- seat " << (i + 1) << ":  " ;
		if (isFree(i))		cout << "<free>" ;
		else                cout << "------  " << *pStr ;
	}
} // CTableBJ::showTable()


void
CTableBJ::showPlayers(const size_t	seat)
{
						CPlayer * pPlayer = nullptr ;

	cout << endl << "Table of Black Jack: " << (m_deck.size() < 52 ? "" : "NEW")
		<< " deck of " << m_deck.size() << " cards" ;
	cout << endl << "------------------------------------------" ;
	if (numPlayers() == 0)    { cout << endl << "-- No players at the Table ..." ; return ; }

	for (size_t i = 0, num = m_aPlayers.capacity() ; i < num ; i++) {
		if (!isFree(i)) {
			cout << endl << (seat != i ? "-  " : "!! "),    // indicate the current one
			pPlayer = &(m_aPlayers[i]) , pPlayer->showPlayer(true) ;
			if (seat == num && !(pPlayer->isBusted())) {	// Show the result for a player that's not busted
										bool	fl = m_house.isBusted() ;
										int		diff = m_house.getTotal() - pPlayer->getTotal() ;
				if (fl)							pPlayer->wins() ;
				else						    {
					if(diff > 0)				pPlayer->loses() ;
					else if (diff == 0)			pPlayer->pushes() ;
					else						pPlayer->wins() ;
				}
			}
		}
	}
	// And, the House
	cout << endl << "------------"
	     << endl << "-  " , m_house.showPlayer(true) ;
} // CTableBJ::showPlayers()

void
CTableBJ::play()
{
						CPlayer *   pPlayer = nullptr ;
						size_t		nSeats = m_aPlayers.capacity() ;
																			// was for a test: cout << endl , showPlayers() , cout << endl ;
	if (numPlayers() == 0)       return ;
	// Clear all hands
	for (size_t j = 0 ; j < nSeats ; j++)    pPlayer = &(m_aPlayers[j]) , pPlayer->clear() ;
	m_house.clear() ; // And the house

	// Deal the initial two  cards
	for (size_t i = 0 ; i < 2 ; i++) {
		for (size_t j = 0 ; j < nSeats ; j++) {
			if (!isFree(j))			{   m_deck.deal(m_aPlayers[j]) ; }
		}
		m_deck.deal(m_house) ;
	}
	m_house.flipFirst() ;
	cout << endl << endl << "-- dealing the 1st round ..." << endl ;

	// Move ont to the 2nd round
	for (size_t j = 0 ; j < nSeats ; j++) {
		if (!isFree(j)) 	{	
			dealSR(&(m_aPlayers[j]), j) ;
		}
	}
	cout << endl << "-- dealing to the House ..." << endl ;
	m_house.flipFirst() , dealSR(&m_house, nSeats) ;
	cout << endl << "-- Finally: " << endl << endl ;
	showPlayers(nSeats) ;	// Makes final  Announcement as well
} // CTableBJ::play()

bool
CTableBJ::dealSR(CPlayer *ptrP, size_t ind)	// Deal the additional cards(until required) to a ptr to CPlayer (Base for CPlayerBJ/CHouseBJ)
{
						bool		fl_bust = false ;
						bool        fl_goon = true ;
	do {
		if (ind < m_aPlayers.capacity()) showPlayers(ind) ;   // It is the House otherwise
		if ((ptrP->getTotal()) == 21)  return(false)  ;
		if (ptrP->requestCard())			m_deck.deal(*ptrP) ;
		else								fl_goon = false ;
	} while (!(fl_bust = ptrP->isBusted()) && fl_goon) ;
	
	return(fl_bust) ;         // Returns if a Player has busted...
} // CTableBJ::dealSR()

// eogame gameBJ.cpp