#include <iostream>
#include <vector>

class BigInteger {
public:
	BigInteger(int);
	friend operator+<>(const BigInteger& b1, const BigInteger& b2);

};

int main() {
	std::cout << "Hello, World!" << std::endl;
	return 0;
}
