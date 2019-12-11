#include <iostream>
#include <vector>

class BigInteger {
public:
	BigInteger() { big = {0}; positive =  true; }
	BigInteger(int val) { big = {static_cast<uint32_t>(std::abs(val))}; positive = val >= 0; }
	BigInteger operator+(const BigInteger& other) const;
	BigInteger operator-(const BigInteger& other) const;
	BigInteger operator*(const BigInteger& other) const;
	BigInteger operator/(const BigInteger& other) const;
	BigInteger operator%(const BigInteger& other) const;
	BigInteger& operator+=(const BigInteger& other);
	BigInteger& operator-=(const BigInteger& other);
	BigInteger& operator*=(const BigInteger& other);
	BigInteger& operator/=(const BigInteger& other);
	BigInteger& operator%=(const BigInteger& other);
	BigInteger operator-() const;
	BigInteger operator++();
	BigInteger operator++(int);
	BigInteger operator--();
	BigInteger operator--(int);
	[[nodiscard]] BigInteger abs() const;
	bool operator==(const BigInteger& other) const;
	bool operator!=(const BigInteger& other) const;
	bool operator<(const BigInteger& other) const;
	bool operator>(const BigInteger& other) const;
	bool operator<=(const BigInteger& other) const;
	bool operator>=(const BigInteger& other) const;
	friend std::ostream& operator<<(std::ostream& os, const BigInteger&) const;
	friend std::istream& operator>>(std::istream& is, BigInteger&);
	std::string toString() const;
	operator int() const;
	operator bool() const;
private:
	[[nodiscard]] size_t parts() const { return big.size(); }
	std::vector<uint32_t> big;
	bool positive;
	constexpr static uint32_t base = 2;
	constexpr static uint32_t max = UINT32_MAX;
	void propagateCarry(size_t pos, int32_t carry, bool subtr);
};

BigInteger& BigInteger::operator+=(const BigInteger& other) {
	if (positive != other.positive && *this < other) {
		*this = other + *this;
	}
	else {
		bool subtr = positive != other.positive;
		uint32_t carry = 0, oldCarry = 0;
		size_t i = 0;
		for (i = 0; i < std::min(parts(), other.parts()); ++i) {
			if (subtr) {
				if ((static_cast<int64_t>(big[i]) - other.big[i]) - oldCarry < 0)
					carry = 1;
				else carry = 0;
				big[i] -= other.big[i];
				big[i] -= oldCarry;
			} else {
				if ((static_cast<int64_t>(big[i]) + other.big[i]) + oldCarry > max)
					carry = 1;
				else carry = 0;
				big[i] += other.big[i];
				big[i] += oldCarry;
			}
			oldCarry = carry;
		}
		if (i == parts() && i < other.parts()) // Happens only in case of addition
			big.insert(big.end(), other.big.begin() + i, other.big.end());
		propagateCarry(i, carry, subtr);
	}
	return *this;
}

void BigInteger::propagateCarry(size_t pos, int32_t carry, bool subtr) {
	while (pos < parts() && carry != 0) {
		if (subtr) {
			big[pos] -= carry;
			carry = (big[pos] == max) ? 1 : 0;
		} else {
			big[pos] += carry;
			carry = (big[pos] == 0) ? 1 : 0;
		}
		++pos;
	}
	if (carry != 0) {
		big.push_back(carry);
	}
}

BigInteger BigInteger::operator+(const BigInteger &other) const {
	BigInteger tmp = *this;
	tmp += other;
	return tmp;
}

BigInteger BigInteger::abs() const {
	BigInteger tmp = *this;
	tmp.positive = true;
	return tmp;
}

bool BigInteger::operator<(const BigInteger &other) const {
	if (!positive && other.positive)
		return true;
	if (positive && !other.positive)
		return false;
	if (!positive && !other.positive) {
		return other.abs() < abs();
	}
	for (int64_t j = std::max(parts(), other.parts()) - 1; j >= 0; --j) {
		if (parts() <= j && other.big[j] != 0)
			return true;
		else if (other.parts() <= j && big[j] != 0)
			return false;
		else if (parts() > j && other.parts() > j) {
			if (big[j] != other.big[j])
				return big[j] < other.big[j];
		}
	}
	return false;
}

int main() {
	BigInteger b1 = -5;
	BigInteger b2 = -10;
	BigInteger b3 = b1 + b2;
	int a = 0;
	return 0;
}
