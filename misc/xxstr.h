#pragma once

#ifndef _XXSTR_NAMESPACE_
#define _XXSTR_NAMESPACE_

#include <iostream>
#include <string>
#include <vector>


//---------------------------------
// XXSTR NAMESPACE
//---------------------------------

namespace xxstr
{
// -------------------------------------------
//  STRING TOKENIZING SECTION
// -------------------------------------------
// As the name section suggest, the following functions (and variables) are for tokenizing strings.
// Tokenizing strings can still be quite slow relatively due to the nature of string and vector memory allocations.
// But still... I've managed to make it fast as I possibly can.

template <typename D>
inline void next_str(std::string& str, D&& delimiters, char& found_delimiter);
   
/** 
*  Tokenize the string. This function is best used when the function is in a loop and is repeatedly used. 
*  This function relies on assigning the string to the occupied vector index rather than constructing a 
*  new one on the vector, the reason why it is needed to resize the vector by at least 10. 
* 
*  @param source_str  - The string to be tokenized
*  @param outVec      - The output vector. outVec must first be resized to at least 10.
*  @param outSize     - The actual size that is occupied by the tokenized strings. The initial variable passed must always be 0
* 
*  @returns True if successful. False if the initial outVec's size is less than 10 or outSize is not 0
*
*  @extra This is slower than token_once when only used once or for a few times
*/
bool token_repeat(const char* source_str, std::vector<std::string>& outVec, int& outSize);

/**
*  Tokenize the string. This function is best used and should probably only used when tokenization is done only once per given time. 
*  This is because of the repeated memory allocations when constructing in a vector. 
* 
*  @param source_str   - The string to be tokenized
*  @param outVec       - The output vector. outVec must be empty before passing it to the function
*  @param reserve_size - The size to reserve the vector capacity for faster insertion. By default, it is set to 10
* 
*  @returns True if successful; False if the initial outVec is not empty
*
*  @extra This is faster than token_repeat when used a few times, but slower if used a lot/repeatedly
*/
bool token_once(const char* source_str, std::vector<std::string>& outVec, int reserve_size = 10);


// -----------------------------------------------------
// STRING UTILITIES FUNCTIONS
// -----------------------------------------------------

// Removes the unneccesary whitespaces from the string
inline void trim(std::string& str);

}


#endif // 
