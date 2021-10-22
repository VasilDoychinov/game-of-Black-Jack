// ownCIN.h: try to tame std::cin
//                     - clean_cin():    cleans unapropriate chars before the next 'read'
//                     - bad_cin():      returns if error bits have been raised for 'cin'. If so - calls clean_cin() 
// placed in "Study\src\include"             -


#pragma once

#include <iostream>


void  clean_cin() ;      // cleans unapropriate chars before the next 'read'
bool  bad_cin() ;        // returns if error bits have been raised for 'cin'. If so - calls clean_cin() 

size_t  inp_Txt(char store[], const size_t const_Max, const char *msg = "") ;
double  getReal(const char *txt = "            enter a number: ") ;


// *** eof ownCIN.h