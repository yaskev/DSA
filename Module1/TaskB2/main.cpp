#include <iostream>
#include <iterator>
#include <vector>

const int alphabetSize = 26;

char firstNotUsed( const std::vector<size_t>& used, const std::string& res ) {
    std::vector<char> letters( alphabetSize, 0 );
    for( size_t u : used ) {
        letters[res[u] - 'a'] = 1;
    }
    // Start with 'b' because 'a' is definitely used
    for( size_t i = 1; i < letters.size(); ++i ) {
        if( !letters[i] ) {
            return static_cast<char>('a' + i);
        }
    }
    return 'b';
}

std::string zFunctionToString( std::istream_iterator<size_t>& iter ) {
    std::string res = "a";
    ++iter;
    bool writeAfterPrefix = true;
    std::vector<size_t> usedLettersIndices;
    std::istream_iterator<size_t> endOfStream;

    while( iter != endOfStream ) {
        if( *iter == 0 ) {
            res += writeAfterPrefix ? firstNotUsed( usedLettersIndices, res ) : 'b';
            writeAfterPrefix = false;
            ++iter;
        } else {
            usedLettersIndices.clear();
            size_t needWrite = *iter;
            size_t written = 0;
            while( needWrite > written ) {
                if( needWrite - written < *iter ) {
                    needWrite = *iter;
                    written = 0;
                    usedLettersIndices.push_back(*iter);
                } else if( needWrite - written == *iter ) {
                    usedLettersIndices.push_back(*iter);
                }
                res += res[written++];
                ++iter;
            }
            writeAfterPrefix = true;
        }
    }

    return res;
}

int main() {
    std::istream_iterator<size_t> iter( std::cin );
    std::cout << zFunctionToString( iter );

    return 0;
}
