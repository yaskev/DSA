#include <iostream>
#include <vector>
#include <array>

const size_t alphabetSize = 256;

size_t getPrefixesNumber(std::string str, char splitter='$') {
    str += splitter;
    std::array<size_t, alphabetSize> counter{};
    std::vector<size_t> classes(str.size(), SIZE_T_MAX);
    std::vector<size_t> suffArray(str.size(), SIZE_T_MAX);
    counter.fill(0);
    for (char c : str) {
        ++counter[c];
    }
    for (size_t i = 1; i < counter.size(); ++i) {
        counter[i] += counter[i-1];
    }
    for (size_t i = 0; i < str.size(); ++i) {
        suffArray[i] = --counter[str[i]];
    }
    size_t classId = 0;
    char lastChar = splitter;
    for (size_t pos : suffArray) {
        if (str[pos] != lastChar) {
            classId += 1;
            lastChar = str[pos];
        }
        classes[pos] = classId;
    }

    for (size_t currLength = 1; currLength <= str.size(); currLength *= 2) {
        std::vector<size_t> partTwoSuff(str.size());
        for (size_t i = 0; i < str.size(); ++i) {
            partTwoSuff[i] = (suffArray[i] + str.size() - currLength) % str.size(); // Mistake?
        }
        counter.fill(0);
        for (size_t elem : partTwoSuff) {
            ++counter[classes[elem]];
        }

        counter[0] = 0;
        for (size_t i = 2; i < str.size(); ++i) {
            counter[i] += counter[i - 1];
        }
        for (size_t i = 0; i < str.size(); ++i) {
            suffArray[counter[classes[partTwoSuff[i]]]++] = partTwoSuff[i];
        }


    }
}

int main() {
    std::string str;
    std::cin >> str;
    std::cout << getPrefixesNumber(str);

    return 0;
}