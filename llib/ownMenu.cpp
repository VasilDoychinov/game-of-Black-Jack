// ownMenu.cpp: to provide presentation and functionality of a simple Menu:
//				functions, etc supporting:
//                - creating
//                - updating
//                - choosing from a Menu
//
// ... related files:
//        - ownMenu.h:  definitions and prototypes
//        - ownCIN.h:   to solve  std::cin issues

#pragma once

#include "ownMenu.h"
#include "ownCIN.h"


CMenu::CMenu(string&& t, string&& choices, const unsigned char delim)
{
							bool   fl = true ;
							size_t len = t.length() ;

	// Set the Title
	if (t == "")     m_Title = ("\n-------------------------------------") ;
	else {
		m_Title = std::move(t) ;
		if (m_Title[len - 1] != ' ')      {						// list of Choices is to follow on a new line
								string  line(len + 2, '-') ;	// to underline the title
			m_Title += std::move('\n' + line + '\n') ;
			fl = false ;
		 }
	}
	m_tChoices = std::move(choices) ;							// As they are formatted

	// Parse the list of choices and set 'm_numEl' and 'm_aEl'
	try {
		if ((m_numEl = _parse(delim)) == 0) {
			m_aEl = nullptr , m_cEl = nullptr ;
			return ;											// There are no Choices indicated - nothing more to do
		}
		m_aEl = new bool[m_numEl] ;
		m_cEl = new unsigned char[m_numEl] ;
	} catch (std::bad_alloc) {
		if (m_aEl != nullptr)          delete [] m_aEl , m_aEl = nullptr ;
		if (m_cEl != nullptr)          delete [] m_cEl , m_cEl = nullptr ;
		m_numEl = 0 ;
	}
	// Set all items Active
	if (m_aEl != nullptr) {
		for (size_t i = 0 ; i < m_numEl ; i++)      m_aEl[i] = true ;
	}
	//Set up the valid choices: use just the character that precedes the delimeter used in parse()
	if (_parseCh(m_cEl, delim) != m_numEl) {
		cout << endl << "__ CMenu: Bad Parse ..." ;
		delete [] m_aEl, delete [] m_cEl ;
		m_cEl = nullptr, m_aEl = nullptr ;
		m_numEl = 0 ;
	}
	return ;
} // CMenu::CMenu()


ostream&
operator<<(ostream& os, const CMenu& menu)
{
	os << menu.m_Title ; 
	// Show the list of choices and, beautify it a little ...
	{				string		sAdj((menu.m_Title).length(), ' ') ;
	for (auto ch : menu.m_tChoices)    {
		cout << ch ;
		if (ch == '\n' && (menu.m_tChoices)[0] != ' ')   cout << sAdj ;
	}
	}

	if (menu.m_aEl == nullptr || menu.m_cEl == nullptr || menu.m_numEl == 0) {  // Empty list of choices: OK just show title ...
																				// os << "\n--- Bad memory ..." ;
		return(os) ;
	}

	size_t     num = menu.m_numEl ;
	if (num > 1 && menu._isLimited()) {
		os << "\n-- available choices: " ;
		for (size_t i = 0 ; i < num ; i++) {
			if ((menu._isActive)(i))       os << '(' << (menu.m_cEl)[i] << ") " ;
		}
	}
	return(os) ;
} // friend CMenu operator<< ()

bool
CMenu::_isLimited() const
{
	for (size_t i = 0 ; i < m_numEl ; i++) {
		if (!(_isActive(i)))       return(true) ;
	}
	return(false) ;
} // CMenu::_isLimited()

size_t
CMenu::_isChoice(const size_t ch) const                // Returns the position[index] of the choice in 'ch'
{
						unsigned char * pCh = m_cEl ;
					
	if (pCh == nullptr)			return(m_numEl) ;

	for (size_t i = 0 ; i < m_numEl ; i++, pCh++) {
		if (*pCh == ch)   break  ;
		else if (isalpha(*pCh)) {
			if (toupper(*pCh) == toupper(ch))   break ;
		}
	}
	return(pCh - m_cEl) ;
} // CMenu::_isChoice()

size_t
CMenu::choice(const bool fl_act, const bool fl_ask) const		// Returns the position of the chosen in the sequence of choices
{
						unsigned char   ch = 0 ;				// of type UC as 'cin >> ' delivers according to the type
						bool     fl_stop = false ;
	do {
		if (m_numEl <= 1)		cout << ">> waiting... enter 0 to go on: " ;  // There might the 'm_numEl' is 0
		else { 
			if (fl_ask)			cout << " >> You choose ? " ;
			else				cout << "   >> " ;
		}

		(std::cin).clear(), std::cin >> ch ;
		fl_stop = bad_cin() ? false : ((ch = (unsigned char)_isChoice(ch)) < m_numEl) ; // 'false' if a !valid input has been made
		if (fl_stop && fl_act)       fl_stop = _isActive(ch) ;
	} while(!fl_stop) ;

	clean_cin() ;
	return(ch) ;
} // CMenu:: choice()


inline bool
CMenu::_isActive(const size_t n) const
{
	return((n >= m_numEl) ? false : (m_aEl[n])) ;
} // CMenu:: _isActive()

bool
CMenu::setActive(const size_t n, const bool fl)
{
	if (n >= m_numEl)           return(false) ;         // Always inactive

	bool   fl_cur = m_aEl[n] ;
	m_aEl[n] = fl ;
	return(fl_cur) ;
} // CMenu:: setActive()

size_t
CMenu::_parse(const unsigned char delim) const
{
				size_t		num = 0 ;
	for (auto ch : m_tChoices) {
		if (ch == delim)         num++ ;
	}
	return(num) ;
} // CMenu:: parse()

size_t
CMenu::_parseCh(unsigned char * store, const unsigned char delim) const
{
	size_t		len = m_tChoices.length() ;
	size_t		num = 0 ;

	if (store != nullptr)   {
		for (size_t i = 1 ; i < len ; i++) {
			if (m_tChoices[i] == delim)      num++, *store++ = m_tChoices[i - 1] ;
		}
	}
	return(num) ;
} // CMenu:: _parseCh()


// *** eof ownMenu.cpp