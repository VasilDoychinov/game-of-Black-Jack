// ownMenu.h: to provide presentation and functionality of a simple Menu:
//				- class definition
//				- function prototypes, etc
// 
//			The idea: - a Menu has a Title - designed in 'tit' and embelished a bit
//					  - ...          list of Choices each described by <c'delim'> in otherwise free text: 'tChoices'
//					  - choice(s) might be 'active' or not
//              


#pragma once

#include <iostream>
#include <string>
#include <new> 

using std::ostream ;
using std::string ;
using std::cout ;
using std::endl ;

class CMenu {                     // Present a list of choices, etc

	friend ostream& operator << (ostream& o, const CMenu& m) ;      // Output an object 

	private:
		string		   m_Title ;				// The Title (what's it about) of a Menu
		string		   m_tChoices ;				// List of all the choices: to be 
		size_t		    m_numEl ;               // # of Items to choose from
		bool          * m_aEl ;                 // m_aEl[i] is true/false if the elements is active or not.
		unsigned char * m_cEl ;                 // the available choices (as characters)

	public:
		explicit CMenu(string&& tit = "", string&& tChoices = "", const unsigned char delim = ')') ;
		virtual  ~CMenu()	{ delete [] m_aEl , delete [] m_cEl ; m_aEl = nullptr , m_cEl = nullptr ; }
		// virtual ~CMenu() is expected to prevent generation of default Copy/Move Constructor op =; C++11

		bool        setActive(const size_t n, const bool fl) ;		// Sets - new status. Returns old status
		size_t		choice(const bool fl_act = true, const bool fl_ask = true) const ;	// Make a valid choice and return the item id

	private:
		size_t		_parse(const unsigned char delim = ')') const ; 
		size_t		_parseCh(unsigned char *store, const unsigned char delim = ')') const ;

		size_t		_isChoice(const size_t ch) const ;		// Returns the # of the option or, 'm_numEl' otherwise
		bool		_isActive(const size_t n) const ;		// Checks if 'n' is an active menu item
		bool		_isLimited() const ;                    // if inactive items exist
};  // class CMenu


ostream& operator << (ostream& os, const CMenu& menu) ;

// *** eof ownMenu.h