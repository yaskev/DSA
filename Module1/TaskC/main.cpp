#include <iostream>
#include <vector>
#include <iterator>
#include <queue>
#include <memory>
#include <unordered_set>
#include <array>

#pragma GCC optimize("Ofast,unroll-all-loops")

class Trie {
public:
    explicit Trie (const std::string& pattern);
    friend std::vector<uint32_t> printMatches(std::istream_iterator<char>, Trie&);
private:
    struct Node;
    std::vector<std::string> split(const std::string& str, char delim='?');
    std::unique_ptr<Node> root;
    void makeAllLinks();
    
    Node *getSufLink (Node*);
    Node *getLink (Node*, char c);
    Node *getCompressedSufLink (Node*);
    
    std::vector<uint32_t> partStartIndex;
    uint32_t patternSize = 0;
    uint32_t doublePatternSize = 0;
    uint32_t pieceNumber = 0;
    static const size_t alphabetSize = 26;
    static const char base = 'a';
};

struct Trie::Node {
    std::array<std::unique_ptr<Node>, alphabetSize> children;
    std::array<Node*, alphabetSize> link{};
    std::unordered_set<int32_t> terminalForIndex;
    Node *parent = nullptr;
    Node *sufLink = nullptr;
    Node *compressedSuffLink = nullptr;
    char sonByChar = 0;
    uint32_t depth = 0; // Only for terminal vertices

    Node(){
        for (int i = 0; i < alphabetSize; ++i) {
            link[i] = nullptr;
        }
    }
};

Trie::Trie( const std::string& pattern ) {
    patternSize = static_cast<uint32_t>(pattern.size());
    std::vector<std::string> parts = split(pattern, '?');

    root = std::make_unique<Node>();
    for( size_t i = 0; i < parts.size(); ++i ) {
        std::string part = parts[i];
        Node *current = root.get();
        for( char c : part ) {
            if( !current->children[c - base] ) {
                auto newChild = std::make_unique<Node>();
                newChild->parent = current;
                newChild->sonByChar = c;
                current->children[c - base] = std::move(newChild);
            }
            current = current->children[c - base].get();
        }
        current->terminalForIndex.insert(partStartIndex[i]);
        current->depth = static_cast<uint32_t>(part.size());
    }
    makeAllLinks();
    doublePatternSize = patternSize * 2;
    pieceNumber = static_cast<uint32_t>(partStartIndex.size());
};

void Trie::makeAllLinks() {
    // Will write bfs
    root->sufLink = root.get();
    root->compressedSuffLink = root.get();
    std::queue<Node*> queue;
    queue.push(root.get());
    while( !queue.empty() ) {
        Node* curr = queue.front();
        queue.pop();
        curr->sufLink = getSufLink(curr);
        curr->compressedSuffLink = getCompressedSufLink(curr);
        for( const auto& it : curr->children ) {
            if( it ) {
                queue.push( it.get() );
            }
        }
    }
}

std::vector<std::string> Trie::split( const std::string& str, char delim ) {
    std::vector<std::string> res;
    std::string part;
    for( size_t i = 0; i < str.size(); ++i ) {
        if( str[i] != delim ) {
            if( part.empty() ) {
                partStartIndex.push_back(i);
            }
            part += str[i];
        } else if( !part.empty() ) {
            res.push_back(part);
            part = "";
        }
    }
    if( !part.empty() ) {
        res.push_back(part);
    }
    return res;
}

Trie::Node* Trie::getLink( Node* node, char c ) {
    if( !node->link[c - base] ) {
        if( node->children[c - base] ) {
            node->link[c - base] = node->children[c - base].get();
        } else if( node == root.get() ) {
            node->link[c - base] = root.get();
        } else {
            node->link[c - base] = getLink(getSufLink(node), c);
        }
    }
    return node->link[c - base];
}

Trie::Node* Trie::getSufLink( Node* node ) {
    if( node->sufLink == nullptr ) {
        if( node == root.get() || node->parent == root.get() ) {
            node->sufLink = root.get();
        } else {
            node->sufLink = getLink( getSufLink(node->parent), node->sonByChar );
        }
    }
    return node->sufLink;
}

inline Trie::Node* Trie::getCompressedSufLink( Node* node ) {
    if( node->compressedSuffLink == nullptr ) {
        if( !getSufLink(node)->terminalForIndex.empty() ) {
            node->compressedSuffLink = getSufLink(node);
        } else if( getSufLink(node) == root.get() ) {
            node->compressedSuffLink = root.get();
        } else {
            node->compressedSuffLink = getCompressedSufLink(getSufLink(node));
        }
    }
    return node->compressedSuffLink;
}

std::vector<uint32_t> printMatches(std::istream_iterator<char> in, Trie& trie) {
    std::istream_iterator<char> endOfStream;
    std::vector<uint32_t> res;
    std::vector<uint32_t> matches( trie.doublePatternSize, 0 );
    uint32_t charNum = 0;
    Trie::Node *curr = trie.root.get();
    while( in != endOfStream ) {
        if( matches[(charNum + trie.patternSize) % (trie.doublePatternSize)] == trie.pieceNumber ) {
            std::cout << charNum - trie.patternSize << " ";
        }
        matches[(charNum + trie.patternSize) % (trie.doublePatternSize)] = 0;
        curr = trie.getLink(curr, *in);
        if( !curr->terminalForIndex.empty() ) {
            for( const int32_t index : curr->terminalForIndex ) {
                if( charNum > trie.patternSize || charNum + 1 >= index + curr->depth ) {
                    matches[(charNum + 1 - (index + curr->depth)) % (trie.doublePatternSize)] += 1;
                }
            }
        }
        Trie::Node *tmp = curr->compressedSuffLink;
        while( tmp != trie.root.get() ) {
            for( const int32_t index : tmp->terminalForIndex ) {
                if( charNum > trie.patternSize || charNum + 1 >= index + tmp->depth ) {
                    matches[(charNum + 1 - (index + tmp->depth)) % (trie.doublePatternSize)] += 1;
                }
            }
            tmp = tmp->compressedSuffLink;
        }
        ++charNum;
        ++in;
    }
    if( charNum >= trie.patternSize ){
        if( matches[(charNum + trie.patternSize) % (trie.doublePatternSize)] == trie.partStartIndex.size() ) {
            std::cout << charNum - trie.patternSize;
        }
    }

    return res;
}

int main() {
    std::cin.tie(nullptr);
    std::ios::sync_with_stdio(false);
    std::string pattern;
    std::cin >> pattern;
    std::istream_iterator<char> istream( std::cin );
    
    Trie trie( pattern );
    std::vector<uint32_t> res = printMatches(istream, trie);
    
    return 0;
}
