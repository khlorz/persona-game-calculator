#include "xxstr.h"

// -----------------------
//     XXSTR NAMESPACE    
// -----------------------
namespace xxstr
{
//----------------------------------------
// STRING TOKENIZING SECTION
//----------------------------------------
/* 
*  These two global variables are implemented for performance reasons:
*  1.) To minimize memory allocations on strings, tokenized strings will be assigned
*    instead to 'token_str' variable rather than creating new string
*  2.) The string to be tokenized will only need to be created once(str_to_tokenize), and every subsequent strings
*    to be tokenized will need to be assigned instead to 'main_str', avoiding memory allocations for creating new strings
*/
static std::string str_to_tokenize;     /**< the string to be tokenized    */
static std::string tokenized_str;       /**< the tokenized string          */

template <typename D>
inline void next_str(std::string& str, D&& delimiters, char& found_delimiter)
{
	const auto pos = str.find_first_of(std::forward<D>(delimiters));

	if (pos == std::string::npos) {
		found_delimiter = '\0';
		trim(str);
		tokenized_str = str;
		str.clear();
		return;
	}

	found_delimiter = str[pos];
	tokenized_str = std::string_view(str.c_str(), pos);
	str.erase(0, pos + 1);
	trim(tokenized_str);
}

bool token_repeat(const char* source_str, std::vector<std::string>& outVec, int& outSize)
{
	if (outVec.size() < 10 || outSize != 0)
		return false;

	str_to_tokenize = source_str;
	bool quote_opened = false;

	while (!str_to_tokenize.empty())
	{
		const char* delimiter_set = quote_opened ? "\"" : ",{}:=\"";
		char delimiter;
		next_str(str_to_tokenize, delimiter_set, delimiter);
		quote_opened = delimiter == '\"' && !quote_opened;

		if (!tokenized_str.empty())
		{
			outVec[outSize] = tokenized_str;
			outSize++;
		}
	}

	return true;
}

bool token_once(const char* source_str, std::vector<std::string>& outVec, int reserve_size)
{
	if (!outVec.empty())
		return false;

	str_to_tokenize = source_str;
	outVec.reserve(reserve_size);
	bool quote_opened = false;

	while (!str_to_tokenize.empty())
	{
		const char* delimiter_set = quote_opened ? "\"" : ",:=\"";
		char delimiter;
		next_str(str_to_tokenize, delimiter_set, delimiter);
		quote_opened = delimiter == '\"' && !quote_opened;

		if (!tokenized_str.empty())
		{
			outVec.emplace_back(tokenized_str);
		}
	}

	return true;
}


// -----------------------------------------------------
// STRING UTILITIES FUNCTIONS
// -----------------------------------------------------

inline void trim(std::string& str)
{
	auto pos_first = str.find_first_not_of(std::forward<std::string_view>(" \t"));

	if (pos_first == std::string::npos)
	{
		str.clear();
		return;
	}

	auto pos_last = str.find_last_not_of(' ');
	str = std::string_view(str.c_str() + pos_first, ++pos_last - pos_first);
}

} // xxstr namespace

