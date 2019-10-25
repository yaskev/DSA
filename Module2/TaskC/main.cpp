#include <iostream>
#include <vector>
#include <array>
#include <string>

const size_t alphabetSize = 256;

// builds a reverse suffix array
std::vector<size_t> reverse (const std::vector<size_t>& suffArray) {
    std::vector<size_t> suffReverse(suffArray.size());
    for (size_t i = 0; i < suffArray.size(); ++i) {
        suffReverse[suffArray[i]] = i;
    }
    return suffReverse;
}

std::vector<size_t> buildLCP(const std::string& str, const std::vector<size_t>& suffArray) {
    std::vector<size_t> result(str.size());
    std::vector<size_t> suffReverse = reverse (suffArray);
    size_t equalNumber = 0;
    for (size_t i = 0; i < str.size(); ++i) {
        if (equalNumber > 0) {
            --equalNumber;
        }
        if (suffReverse[i] + 1 == str.size()) {
            result[suffReverse[i]] = 0;
            equalNumber = 0;
            continue;
        } else {
            size_t j = suffArray[suffReverse[i] + 1];
            while (std::max(i + equalNumber, j + equalNumber) < str.size()
                   && str[i + equalNumber] == str[j + equalNumber]) {
                ++equalNumber;
            }
            result[suffReverse[i]] = equalNumber;
        }
    }

    return result;
}

std::vector<size_t> recalculateClasses(const std::vector<size_t>& suffArray,
                                       const std::vector<size_t>& oldClasses,
                                       size_t currLength, size_t strLength) {
    std::vector<size_t> classes(strLength);
    size_t classId = 0;
    classes[suffArray[0]] = 0;
    for (size_t i = 1; i < strLength; ++i) {
        // Since elems in suffArray are sorted, we can check only (in)equality
        size_t mid1 = (suffArray[i] + currLength) % strLength;
        size_t mid2 = (suffArray[i - 1] + currLength) % strLength;
        if (oldClasses[suffArray[i]] != oldClasses[suffArray[i - 1]]
            || oldClasses[mid1] != oldClasses[mid2]) {
            ++classId;
        }
        classes[suffArray[i]] = classId;
    }

    return classes;
}

// This is a generalized counting sort
// When we sort the string (for the first time), wrapper is an identity transformation (wrapper[i] = i)
// When we sort first parts of substrings (all later times), wrapper[i] = classes[i]
template <typename T>
std::vector<size_t> doCountingSort(const T& array, const std::vector<size_t>& wrapper, bool byParts=false) {
    std::vector<size_t> result(array.size());
    std::vector<size_t> counter(std::max(alphabetSize, array.size()), 0);
    for (const auto elem : array) {
        ++counter[wrapper[elem]];
    }
    for (size_t i = 1; i < counter.size(); ++i) {
        counter[i] += counter[i - 1];
    }
    for (size_t i = 0; i < array.size(); ++i) {
        result[--counter[wrapper[array[array.size() - i - 1]]]] =
                byParts ? array[array.size() - 1 - i] : array.size() - 1 - i;
    }

    return result;
}

std::vector<size_t> buildSuffArray (std::string str, char splitter='$') {
    str += splitter;
    std::vector<size_t> classes(str.size(), 0);
    // Identity trasformation to reuse doCountingSort
    std::vector<size_t> wrapper(alphabetSize);
    for (size_t i = 0; i < alphabetSize; ++i) {
        wrapper[i] = i;
    }
    std::vector<size_t> suffArray = doCountingSort(str, wrapper);
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
        std::vector<size_t> rightHandSuff(str.size());
        // Sort by LSD
        for (size_t i = 0; i < str.size(); ++i) {
            rightHandSuff[i] = (suffArray[i] + str.size() - currLength) % str.size();
        }
        // Now sort by MSD, classes is a wrapper tansformation
        suffArray = doCountingSort(rightHandSuff, classes, true);
        classes = recalculateClasses(suffArray, classes, currLength, str.size());
    }

    return suffArray;
}

std::string getStringInOrder(const std::string& first,
                             const std::string& second, size_t  k,
                             char midSplitter='$', char endSplitter='#') {
    std::string str = first + midSplitter + second;
    std::vector<size_t> suffArray = buildSuffArray(str, endSplitter);
    str += endSplitter;
    std::vector<size_t> reversed = reverse(suffArray);
    std::vector<size_t> lcp = buildLCP(str, suffArray);
    size_t currMin = 0; // Already written number of substrings

    // Start loop with 2 since 0 and 1 are splitters
    for (size_t i = 2; i + 1 < str.size(); ++i) {
        // If the chars are from different strings, then do smth
        if ((suffArray[i] < first.size() && suffArray[i + 1] > first.size())
            || (suffArray[i] > first.size() && suffArray[i + 1] < first.size())) {
            if (k + currMin <= lcp[i]) {
                return str.substr(suffArray[i], k + currMin);
            }
            if (lcp[i] > currMin) {
                k -= (lcp[i] - currMin);
                currMin = lcp[i];
            }
        }
        if (lcp[i] < currMin) {
            currMin = lcp[i];
        }
    }
    return "-1";
}

int main() {
    std::string first, second;
    size_t k = 0;
    std::cin >> first >> second;
    std::cin >> k;
    std::cout << getStringInOrder(first, second, k);

    return 0;
}
