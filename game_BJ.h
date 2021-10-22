// game_BJ.h: 
//			- classes:
//				. to describe a card of Black Jack in: 			classCard.[h/cpp]
//				.  ...           hand                :          classHand.h
//				.  ...     gamplers and the House    :			classPlayer.h
//

#pragma once

#include <stdio.h>

#include "classCard.h"
#include "classHand.h"
#include "classPlayer.h"
#include "llib\vArray.h"

constexpr size_t MAX_NUM_PLAYERS = 7 ;      // The maximum number of players per Table	

class CTableBJ {
	private:
		varray<CPlayerBJ>		m_aPlayers ;     // All seats, actually: a seat is free if 'm_nameP' is ""
		CHouseBJ				m_house ;
		CDeck					m_deck ;

		size_t					m_nP ;

	public:
		explicit CTableBJ() : m_aPlayers(MAX_NUM_PLAYERS, CPlayerBJ(), MAX_NUM_PLAYERS),
							  m_house(), m_deck(), m_nP(0)
						{ cout << "\n\n\n__ Welcome to the game of Black Jack (ver 1.0) ..." << endl ; }
		virtual ~CTableBJ() { cout << endl << "\n__ Game Over ..." << endl << endl ; }

		CTableBJ(const CTableBJ& tbj) = delete ; // { cout << "\n__ CCTableBJ: to prevent the default..." ; m_nP = tbj.m_nP ; }
		CTableBJ& operator =(const CTableBJ& tbj) = delete ; // { cout << "\n__ CTableBJ = &: to prevent the default..." ; }

		void	showTable() const ;		// List of Players, seats and Deck

		size_t	numPlayers() const      { return(m_nP) ; }       // the number of Players at ...
		size_t	nVacant() const 		{ return(m_aPlayers.capacity() - m_nP) ; }
		bool    addPlayer() ;           // a New player to enter
		bool	addRPlayers() ;         // Add random # of players to rnadom free seats 
		bool	removePlayer() ;        // a seat to be freed
		void	play() ;


	private:
		bool	isFree(const size_t seat) const ;
		size_t	chooseSeat(bool fl_vac) const ;
		void	_addPlayer(CPlayer * pPtr, string&& name) ;
	// public:     // To test
		void	showPlayers(size_t seat) ;	// List of Players, their hands and score
		bool	dealSR(CPlayer *ptrP, size_t ind) ;	// Deal the additional cards(until required) to a ptr to CPlayer 
}; // class CTableBJ

/* eof game_BJ.h */