#include <iostream>
#include <vector>
#include <iterator>

template <typename PatternType, typename ElemType>
size_t reusePrefixFunction( size_t compIndex,  const std::vector<size_t>& prefix,
                            const PatternType& pattern,
                            ElemType valueRequired ) {
    while( compIndex > 0 && valueRequired != pattern[compIndex] ) {
        compIndex = prefix[compIndex - 1];
    }
    return valueRequired == pattern[compIndex] ? compIndex + 1 : 0;
}

template <typename PatternType>
std::vector<size_t> computePrefixFunction( const PatternType& pattern ) {
    std::vector<size_t> prefix(pattern.size());
    prefix[0] = 0;
    // Start with 1 because prefix[0] = 0 by definition
    for( size_t i = 1; i < pattern.size(); ++i ) {
        prefix[i] = reusePrefixFunction(prefix[i - 1], prefix, pattern, pattern[i]);
    }
    return prefix;
}

template <typename BeginIterator, typename EndIterator, typename OutputIterator,
        typename PatternType>
void printMatches( const PatternType& pattern, BeginIterator begin, EndIterator end,
                   OutputIterator out ) {
    std::vector<size_t> prefix = computePrefixFunction(pattern);
    size_t prevPos = 0; // The position we ended at before
    size_t indexCounter = 0; // Aka string index
    while( begin != end ) {
        size_t compIndex = reusePrefixFunction(prevPos, prefix, pattern, *begin);
        // We found a match!
        if( compIndex == pattern.size() ) {
            // The index i - the length of pattern and "#"
            *out = indexCounter - pattern.size() + 1;
            ++out;
        }
        prevPos = compIndex;
        ++begin;
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
    std::istream_iterator<char> eof;

    printMatches(pattern, in, eof, out);

    return 0;
}
