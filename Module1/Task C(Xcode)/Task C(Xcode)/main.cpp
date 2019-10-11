#include <iostream>
#include <vector>
#include <iterator>
#include <queue>
#include <memory>
#include <unordered_set>

#pragma GCC optimize("Ofast,unroll-all-loops")

class Trie {
public:
    explicit Trie( const std::vector<std::string>& parts, uint32_t patternSize_ );
    explicit Trie( const std::string& pattern );
    std::vector<uint32_t> findMatches( std::istream_iterator<char> );
private:
    struct Node;
    void Init( const std::vector<std::string>& parts ); // To support 2 constructors
    std::vector<std::string> split( const std::string& str, char delim='?' );
    std::unique_ptr<Node> root = nullptr;
    void makeAllLinks();
    
    Node *getSufLink( Node* );
    Node *getLink( Node*, char c );
    Node *getCompressedSufLink( Node* );
    
    std::vector<uint32_t> partStartIndex;
    uint32_t patternSize = 0;
    static const uint32_t alphabetSize = 26;
};

struct Trie::Node {
    //    std::map<char, std::unique_ptr<Node>> children;
    //    std::map<char, Node*> link;
    std::vector<std::unique_ptr<Node>> children = std::vector<std::unique_ptr<Node>>( alphabetSize );
    std::vector<Node*> link = std::vector<Node*>( alphabetSize );
    std::unordered_set<uint32_t> terminalForIndex;
    Node *parent = nullptr;
    Node *sufLink = nullptr;
    Node *compressedSuffLink = nullptr;
    char sonByChar = 0;
    uint32_t depth = 0; // Only for terminal vertices
};

void Trie::Init( const std::vector<std::string>& parts ) {
    root = std::make_unique<Node>();
    for( uint32_t i = 0; i < parts.size(); ++i ) {
        std::string part = parts.at(i);
        Node *current = root.get();
        for( char c : part ) {
            if( !current->children.at(c - 'a') ) {
                auto newChild = std::make_unique<Node>();
                newChild->parent = current;
                newChild->sonByChar = c;
                current->children.at(c - 'a') = std::move(newChild);
            }
            current = current->children.at(c - 'a').get();
        }
        current->terminalForIndex.insert(partStartIndex.at(i));
        current->depth = part.size();
    }
    makeAllLinks();
}

Trie::Trie( const std::string& pattern ) {
    patternSize = pattern.size();
    Init( split(pattern, '?') );
};

Trie::Trie( const std::vector<std::string>& parts, uint32_t patternSize_ ) {
    patternSize = patternSize_;
    Init( parts );
}

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

std::vector<uint32_t> Trie::findMatches( std::istream_iterator<char> in ) {
    std::istream_iterator<char> endOfStream;
    std::vector<uint32_t> res;
    std::vector<uint32_t> matches(4 * patternSize, 0);
    uint32_t charNum = 0;
    Node *curr = root.get();
    while( in != endOfStream ) {
        curr = getLink(curr, *in);
        if( !curr->terminalForIndex.empty() ) {
            for( const uint32_t index : curr->terminalForIndex ) {
                if( charNum + 1 >= index + curr->depth ) {
                    matches.at(charNum + 1 - (index + curr->depth)) += 1;
                }
            }
        }
        Node *tmp = curr->compressedSuffLink;
        while( tmp != root.get() ) {
            for( const uint32_t index : tmp->terminalForIndex ) {
                if( charNum + 1 >= index + tmp->depth ) {
                    matches.at(charNum + 1 - (index + tmp->depth)) += 1;
                }
            }
            tmp = tmp->compressedSuffLink;
        }
        ++charNum;
        ++in;
        if( matches.size() == charNum ) {
            matches.resize(2 * charNum, 0);
        }
    }
    if( charNum >= patternSize ) {
        for( uint32_t i = 0; i < charNum - patternSize + 1; ++i ) {
            if( matches.at(i) >= partStartIndex.size() ) {
                res.push_back(i);
            }
        }
    }
    
    return res;
}

std::vector<std::string> Trie::split( const std::string& str, char delim ) {
    std::vector<std::string> res;
    std::string part;
    for( uint32_t i = 0; i < str.size(); ++i ) {
        if( str.at(i) != delim ) {
            if( part.empty() ) {
                partStartIndex.push_back(i);
            }
            part += str.at(i);
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
    if( !node->link.at(c - 'a') ) {
        if( node->children.at(c - 'a') ) {
            node->link.at(c - 'a') = node->children.at(c - 'a').get();
        } else if( node == root.get() ) {
            node->link.at(c - 'a') = root.get();
        } else {
            node->link.at(c - 'a') = getLink(getSufLink(node), c);
        }
    }
    return node->link.at(c - 'a');
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

Trie::Node* Trie::getCompressedSufLink( Node* node ) {
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

int main() {
    std::cin.tie(nullptr);
    std::ios::sync_with_stdio(false);
    std::string pattern;
    std::cin >> pattern;
    std::istream_iterator<char> in( std::cin );
    
    Trie trie( pattern );
    for( uint32_t entry : trie.findMatches(in) ) {
        std::cout << entry << " ";
    }
    
    
    return 0;
}


