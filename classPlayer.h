// classPlayer.h: contains descriptions, etc for the following classes
//				. a Player in general
//				. a Human player
//				. a Computer player as the House (for now)
//

#pragma once

#include <string>

#include "classHand.h"

const size_t MaxNameLen = 13 ;          // ...         size of a player's name, incl. '\0'

class CPlayer : public CHand {

	protected:
			std::string	 m_nameP ;

	public:
		explicit CPlayer(const string& name = "", const size_t hs = 12) : CHand(hs), m_nameP(name)
						{ /*cout << "\n__ CPlayer: <" << m_nameP << "> to hold max " << hs << " cards ..." ;*/ }
		virtual   ~CPlayer() { /*cout << "\n__ ~ CPlayer <" << m_nameP << "> ..." ;*/ }

		virtual CPlayer&  operator =(string&& name)  noexcept ;
		virtual CPlayer&  operator =(const string & name) ;

		virtual void showPlayer(bool fl_all = false) ; // const ;
		virtual bool requestCard() const = 0 ;
		virtual void loses() const = 0 ;
		virtual void pushes() const = 0 ;
		virtual void wins() const = 0 ;

		bool		isBusted() const { return (getTotal() > 21) ; }

	// protected:
		const string* const	nameP() const { return(&m_nameP) ; }
		friend std::ostream& operator <<(ostream& os, const CPlayer& pl) { os << pl.m_nameP ; return(os) ; }
}; // class CPlayer


class CPlayerBJ : public CPlayer {				// To represent the 'Human' players
	public:
		explicit CPlayerBJ(const string name = "") : CPlayer(name, 12) { }
		virtual ~CPlayerBJ() { } ;

		virtual bool	requestCard() const override ;

		virtual void loses() const override		{ cout << std::setw(20) << "\t--- loses ..." ; }     // Announce what has happend
		virtual void pushes() const override	{ cout << std::setw(20) << "\t--- Pushes ..." ; }
		virtual void wins() const override 		{ cout << std::setw(20) << "\t--- WINs ..." ; }
}; // class CPlayerBJ

class CHouseBJ : public CPlayer {			// To represent the House(bank)
	public:
	explicit CHouseBJ() : CPlayer("the House", 12) { }
	virtual ~CHouseBJ() { }

		virtual bool	requestCard() const override { return(getTotal() <= 16) ; }
		void			flipFirst() { if (!isEmpty())  (m_store[0])->flip() ; }

		virtual void loses() const override { cout << " --- But, the House never loses ..." ; }     // Announce what has happend
		virtual void pushes() const override { cout << " --- the House Pushes (sometimes) ..." ; }
		virtual void wins() const override { cout << " --- And, the House always WINs ..." ; }
} ; // class CHouseBJ


// eof classPlayer.h