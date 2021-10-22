// classPlayer.cpp: member functions of
//				. CPlayer:		derived from CHand, Base for CPlayerBJ, base for CHouseBJ
//				. CPlayerBJ:						derived from CPlayer
//				. CHouseBJ:		..								 CPlayer
//


#include "llib\ownMenu.h"

#include "classHand.h"
#include "classPlayer.h"


// CPlayer member functions follow

CPlayer&
CPlayer::operator =(string && name) noexcept
{
															// cout << "__ CPlayer = && with " << name << " ..." ;
	m_nameP = std::move(name) ;
	return(*this) ;
} // CPlayer::operator =(&&)

CPlayer&
CPlayer::operator =(const string& name)
{
															// cout << "__ CPlayer = & with " << name << " ..." ;
	m_nameP = name ;
	return(*this) ;
} // CPlayer::operator =(&)

void
CPlayer::showPlayer(bool fl_all)
{
	CHand * pHand = this ;

	cout << m_nameP ;
	if (fl_all) {
		for (size_t i = (m_nameP).length() ; i < MaxNameLen ; i++)     cout << ' ' ;
		cout << '(' << std::setw(2) << getTotal() << ')' << " - " << *pHand  // all follow, eventually..." ; 
		     << (isBusted() ? " Busts." : "") ;
	}
} // CPlayer::showPlayer()

// eol   CPlayer

// CPlayerBJ member functions follow

bool
CPlayerBJ::requestCard() const
{
	CMenu  mConf(", do you want a hit? ", "y)es or n)o") ;
	cout << endl << "\n> " << *nameP() << mConf ;
	return(mConf.choice(true, false) == 0) ;
} // CPlayerBJ::requestCard()

// eol  CPlayerBJ

// eof  classPlayer.cpp