#include <iostream>
#include <vector>
#include <iterator>

std::vector<size_t> computePrefixFunction( const std::string& pattern ) {
    std::vector<size_t> prefix(pattern.size());
    prefix[0] = 0;
    // Start with 1 because prefix[0] = 0 by definition
    for( size_t i = 1; i < pattern.size(); ++i ) {
        size_t compIndex = prefix[i - 1];
        while( compIndex > 0 && pattern[i] != pattern[compIndex] ) {
            compIndex = prefix[compIndex - 1];
        }
        prefix[i] = (pattern[i] == pattern[compIndex]) ? (compIndex + 1) : 0;
    }
    return prefix;
}

void printMatches( const std::string& pattern, std::istream_iterator<char>& in,
                   std::ostream_iterator<size_t>& out ) {
    std::vector<size_t> prefix = computePrefixFunction(pattern);
    std::istream_iterator<char> endOfStream;
    size_t prevPos = 0; // The position we ended at before
    size_t indexCounter = 0; // Aka string index
    while( in != endOfStream ) {
        size_t compIndex = prevPos;
        // Find the largest index where we can reuse prefix function
        while( compIndex > 0 && pattern[compIndex] != *in ) {
            compIndex = prefix[compIndex - 1];
        }
        compIndex = (pattern[compIndex] == *in) ? (compIndex + 1) : 0;
        // We found a match!
        if( compIndex == pattern.size() ) {
            // The index i - the length of pattern and "#"
            *out = indexCounter - pattern.size() + 1;
        }
        prevPos = compIndex;
        ++in;
        ++indexCounter;
    }
}

int main() {
    std::cin.tie(nullptr);
    std::ios::sync_with_stdio(false);

    std::string pattern;
    std::cin >> pattern;

    std::ostream_iterator<size_t> out( std::cout, " " );
    std::istream_iterator<char> in( std::cin );

    printMatches(pattern, in, out);

    return 0;
}
