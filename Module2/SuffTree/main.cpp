#include <iostream>
#include <vector>
#include <iterator>
#include <unordered_map>

const static size_t inf = 18446744073709551615;

template <typename Iter, typename Char>
class SuffTree {
public:
    SuffTree(Iter in, Iter eof);
    void print();

private:
    const static size_t initialTreeSize = 100;
    struct Node;
    std::vector<Node> tree = std::vector<Node>(initialTreeSize);
    std::string str;
    std::pair<size_t, size_t> getLink(size_t currVertex, Char c);
    std::pair<size_t, size_t> go(const std::pair<size_t, size_t>& position, Char c);
};

template <typename Iter, typename Char>
struct SuffTree<Iter, Char>::Node {
    size_t parent = 0;
    size_t left = 0;
    size_t right = inf; // The next after last character
    size_t link = inf; // Suffix link
    std::unordered_map<Char, size_t> children;
};

template <typename Iter, typename Char>
std::pair<size_t, size_t> SuffTree<Iter, Char>::go(const std::pair<size_t, size_t>& position, Char c) { // Не только лишь по листам надо проходиться как по массиву
    if (position.second < tree[position.first].right) {
        return std::make_pair(position.first, position.second + 1);
    }
    return std::make_pair(tree[position.first].children[c], 0);
}

template <typename Iter, typename Char>
std::pair<size_t, size_t> SuffTree<Iter, Char>::getLink(size_t currVertex, Char c) {
    std::vector<Char> path;
    size_t curr = tree[currVertex].link;

    for (size_t i = 0; i <= tree[currVertex].right; ++i) { // Mistake here
        path.push_back(str[tree[currVertex].right - i]);
    }

    while (curr == inf) {
        size_t index = tree[currVertex].right;
        for (size_t i = 0; i <= index; ++i) { // ANd mistake here AND EVERYWHERE!!!!!!!
            path.push_back(str[tree[currVertex].right - i]);
        }
        currVertex = tree[currVertex].parent;
        curr = tree[currVertex].link;
    }

    currVertex = curr;
    path.pop_back();

    std::pair<size_t, size_t> position;
    for (int64_t i = path.size() - 1; i >= 0; --i) {
        position = go(position, path[i]);
    }
    return position;
}

template <typename Iter, typename Char>
SuffTree<Iter, Char>::SuffTree(Iter in, Iter eof) {
    tree.emplace_back(0, 0, 0, 0); // Add root
    size_t currVertex = 0;
    size_t currShift = 0;
    size_t index = 0;
    while (in != eof) {
        str += *in;
        bool finished = false;

        while (!finished) {
            if (tree[currVertex].children.find(*in) != tree[currVertex].end()) {
                currVertex = tree[currVertex].children[*in];
                currShift = 0;
                finished = true;
            } else if (tree[currVertex].left + currShift + 1 < index && str[tree[currVertex].left + currShift + 1] == *in) {
                currShift += 1;
                finished = true;
            }
            else { // No way to the given char
                tree.emplace_back(currVertex, index, inf, inf);
                tree[currVertex].right = currShift;
                tree[currVertex].children[*in] = tree.size() - 1;
                if (tree[currVertex].children.size() == 1) { // This was a list
                    tree.emplace_back(currVertex, tree[currVertex].left + currShift + 1, inf, inf);
                    tree[currVertex].children[*in] = tree.size() - 1;
                }

                tree[currVertex].link = getLink(currVertex, *in);
                size_t tmp = currVertex;
                currVertex = tree[tmp].link.first;
                currShift = tree[tmp].link.second;
            }
        }
        ++in;
        ++index;
    }
}

template <typename Iter, typename Char>
void SuffTree<Iter, Char>::print() {

}

int main(int argc, const char * argv[]) {
    std::istream_iterator<char> in (std::cin);
    std::istream_iterator<char> eof;
    SuffTree<std::istream_iterator<char>, char> tree (in, eof);
    tree.print();

    return 0;
}
