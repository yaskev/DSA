#include <iostream>
#include <vector>
#include <map>
#include <iterator>

class Trie {
public:
    explicit Trie( const std::vector<std::string>& parts );
    explicit Trie( const std::string& pattern );
    std::vector<size_t> findMatches( std::istream_iterator<char>& ) const;
private:
    struct Node;
    void Init( const std::vector<std::string>& parts ); // To support 2 constructors
    std::vector<std::string> split( const std::string& str, char delim='?' );
    Node *root = nullptr;
    void makeSufLinks();
    void makeCompressedSufLinks();

    Node *getSufLink( Node* );
    Node *getLink( Node*, char c );
    Node *getCompressedSufLink( Node* );

    std::vector<size_t> partStartIndex;
};

struct Trie::Node {
    std::map<char, Node*> children;
    std::map<char, Node*> link;
    size_t terminalForIndex = -1;
    Node *parent = nullptr;
    Node *sufLink = nullptr;
    Node *compressedSuffLink = nullptr;
    char sonByChar = 0;
};

void Trie::Init( const std::vector<std::string>& parts ) {
    root = new Node();
    root->sufLink = root;
    size_t currIndex = 0;
    for( const std::string& part : parts ) {
        Node *current = root;
        for( char c : part ) {
            if( current->children.find(c) == current->children.end() ) {
                Node *newChild = new Node();
                newChild->parent = current;
                newChild->sonByChar = c;
                current->children.emplace(c, newChild);
            }
            current = current->children[c];
        }
        current->terminalForIndex = currIndex;
        currIndex += part.size() + 1;
    }
    makeSufLinks();
    makeCompressedSufLinks();
}

Trie::Trie( const std::string& pattern ) {
    Init( split(pattern, '?') );
};

Trie::Trie( const std::vector<std::string>& parts ) {
    Init( parts );
}

void Trie::makeCompressedSufLinks() {

}

void Trie::makeSufLinks() {

}

std::vector<size_t> Trie::findMatches( std::istream_iterator<char>& text ) const {
    std::vector<size_t> res;


    return res;
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
    if( node->link.find(c) == node->link.end() ) {
        if( node->children.find(c) != node->children.end() ) {
            node->link[c] = node->children[c];
        } else if( node == root ) {
            node->link[c] = root;
        } else {
            node->link[c] = getLink(getSufLink(node), c);
        }
    }
    return node->link[c];
}

Trie::Node* Trie::getSufLink( Node* node ) {

}

Trie::Node* Trie::getCompressedSufLink( Node* node ) {

}

int main() {
    std::string pattern;
    std::cin >> pattern;
    std::istream_iterator<char> text( std::cin );
    Trie trie( pattern );

    for( size_t entry : trie.findMatches(text) ) {
        std::cout << entry << " ";
    }

    return 0;
}