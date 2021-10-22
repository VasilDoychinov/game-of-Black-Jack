// ownCIN.cpp: std::cin related functions (see ownCIN.h)

#include <iostream>
#include "ownCIN.h"

void
clean_cin()
{
    const size_t      line_size = 5 ;
    char              line[line_size] = { 0, } ;

    do {
        (std::cin).clear() ;
        (std::cin).getline (line, line_size, '\n') ;    //  (char x = 0 ; x != '\n' ;  )       cin >> x ; Does NOT work
    } while ((std::cin).rdstate() & std::ios::failbit) ;
    (std::cin).clear(std::ios::failbit) ;
    return ;
} // clean_cin()

bool
bad_cin()
{
    if ((std::cin).fail()) {
        std::cout << std::endl << "--> unexpected character(s) ..." ;
        // There is a non-appropriate text entered. Remove the whole string from the INPUT
        clean_cin() ;
        return(true) ;    // to continue reading the input
    }
    return(false) ;   // Adequate value was entered-> check it
} // bad_cin()


size_t
inp_Txt(char store[], const size_t const_Max, const char *msg) // Gets in a text of upto const_Max, incl the trailing 0, characters. 
{																    // Returns the length of what entered
                    // cout << endl << (const_Max - 1) << " characters allowed..." << endl << msg ;
    std::cout << msg ;
    if (const_Max > 0)                   store[0] = 0 ;
    if (const_Max < 2)                   return(0) ;

    // Skip the eventual '\n' leftovers from eventual previous input 
    do {
        (std::cin).clear() , (std::cin).getline(store, const_Max, '\n') ;
    } while (strlen(store) == 0) ;

    if ((std::cin).rdstate() & std::ios::failbit) {    // An Exeption was thrown: failbit
        std::cout << std::endl << "--->> text too long. Clearing the oversize..." << std::endl ;
        clean_cin() ;
    }

    return(strlen(store)) ;
} // inp_Txt()


double 
getReal(const char *txt)
{
    double      d = -1. ; // T   d ;

    do {            // Read stream until a proper value is entered
        std::cout << txt ; 
        (std::cin).clear(), std::cin >> d ;
    } while (bad_cin()) ;

    clean_cin() ;
    return(d) ;
} // getNumber(l)



// *** eof ownCIN.cpp