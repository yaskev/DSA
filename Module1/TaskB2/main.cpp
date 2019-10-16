include <iterator>
#include <vector>

const size_t alphabetSize = 26;
const char firstChar = 'a';

template <typename TextType>
char firstNotUsed( const std::vector<size_t>& used, const TextType& res ) {
    std::vector<char> letters( alphabetSize, 0 );
    for( size_t u : used ) {
        letters[res[u] - firstChar] = 1;
    }
    // Start with 'b' because 'a' is definitely used
    for( size_t i = 1; i < letters.size(); ++i ) {
        if( !letters[i] ) {
            return static_cast<char>(firstChar + i);
        }
    }
    return firstChar + 1;
}

template <typename BeginIterator, typename EndIterator>
std::string zFunctionToString( BeginIterator begin, EndIterator end ) {
    std::string res = "a";
    ++begin;
    bool writeAfterPrefix = true;
    std::vector<size_t> usedLettersIndices;

    while( begin != end ) {
        if( *begin == 0 ) {
            res += writeAfterPrefix ? firstNotUsed( usedLettersIndices, res ) : static_cast<char>(firstChar + 1);
            writeAfterPrefix = false;
            ++begin;
        } else {
            usedLettersIndices.clear();
            size_t needWrite = *begin;
            size_t written = 0;
            while( needWrite > written ) {
                if( needWrite - written < *begin ) {
                    needWrite = *begin;
                    written = 0;
                    usedLettersIndices.push_back(*begin);
                } else if( needWrite - written == *begin ) {
                    usedLettersIndices.push_back(*begin);
                }
                res += res[written++];
                ++begin;
            }
            writeAfterPrefix = true;
        }
    }

    return res;
}

int main() {
    std::istream_iterator<size_t> iter( std::cin );
    std::istream_iterator<size_t> eof;
    std::cout << zFunctionToString( iter, eof );

    return 0;
}
