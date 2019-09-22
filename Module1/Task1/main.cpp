#include <iostream>
#include <vector>

std::vector<uint32_t> computePrefixFunction( const std::string& pattern ) {
    uint32_t plen = pattern.size();
    std::vector<uint32_t> prefix(plen);
    prefix[0] = 0;
    uint32_t compIndex = 0;

    // Start with 1 because prefix[0] = 0 by definition
    for( uint32_t i = 1; i < plen; ++i ) {
        compIndex = prefix[i - 1];
        while( compIndex > 0 && pattern[i] != pattern[compIndex] ) {
            compIndex = prefix[compIndex - 1];
        }
        prefix[i] = (pattern[i] == pattern[compIndex]) ? (compIndex + 1) : 0;
    }
    return prefix;
}

std::vector<uint32_t> match( const std::string& str, const std::string& pattern ) {
    std::vector<uint32_t> prefix = computePrefixFunction(pattern);
    std::vector<uint32_t> entries; // Will put matching indices here
    uint32_t slen = str.size();
    uint32_t plen = pattern.size();
    uint32_t compIndex = 0;
    uint32_t prevPos = 0; // The position we ended at before
    for( uint32_t i = 0; i < slen; ++i ) {
        compIndex = prevPos;
        // Find the largest index where we can reuse prefix function
        while( compIndex > 0 && pattern[compIndex] != str[i] ) {
            compIndex = prefix[compIndex - 1];
        }
        compIndex = (pattern[compIndex] == str[i]) ? (compIndex + 1) : 0;
        // We found a match!
        if( compIndex == plen ) {
            // The index i - the length of pattern and "#"
            entries.push_back(i - plen + 1);
        }
        prevPos = compIndex;
    }
    return entries;
}

int main() {
    std::string pattern;
    std::string str;
    std::cin >> pattern;
    std::cin >> str;

    for( uint32_t i : match(str, pattern) ) {
        std::cout << i << " ";
    }

    return 0;
}
