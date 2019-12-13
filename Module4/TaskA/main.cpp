#include <iostream>
#include <vector>

class BigInteger {
public:
	BigInteger() { big = {0}; positive = true;}
	BigInteger(int val) { big = {static_cast<uint32_t>(std::abs(val))}; positive = val >= 0;}
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
	BigInteger& operator++();
	const BigInteger operator++(int);
	BigInteger& operator--();
	const BigInteger operator--(int);
	[[nodiscard]] BigInteger abs() const;
	bool operator==(const BigInteger& other) const;
	bool operator!=(const BigInteger& other) const;
	bool operator<(const BigInteger& other) const;
	bool operator>(const BigInteger& other) const;
	bool operator<=(const BigInteger& other) const;
	bool operator>=(const BigInteger& other) const;
	friend std::ostream& operator<<(std::ostream& os, const BigInteger&);
	friend std::istream& operator>>(std::istream& is, BigInteger&);
	[[nodiscard]] std::string toString() const;
	explicit operator bool() const;
	explicit operator int() const {return big[0] * (positive ? 1 : -1); }
private:
	void fix();
	BigInteger(uint32_t val) {big = {val}; positive = true; }
	[[nodiscard]] size_t parts() const { return big.size(); }
	std::vector<uint32_t> big;
	bool positive;
	constexpr static uint32_t base = 2;
	constexpr static uint32_t max = UINT32_MAX;
	void propagateCarry(size_t pos, int32_t carry, bool subtr);
};

void BigInteger::fix() {
	for (int64_t i = big.size() - 1; i > 0; --i) {
		if (big[i] == 0)
			big.erase(big.begin() + i);
		else
			break;
	}
}

BigInteger& BigInteger::operator+=(const BigInteger& other) {
	if (positive != other.positive && this->abs() < other.abs()) {
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
		fix();
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

BigInteger BigInteger::operator-() const {
	BigInteger tmp = *this;
	tmp.positive = !tmp.positive;
	return tmp;
}

BigInteger::operator bool() const {
	for (uint32_t val : big) {
		if (val != 0)
			return true;
	}
	return false;
}

BigInteger BigInteger::operator-(const BigInteger &other) const {
	return *this + (-other);
}

BigInteger& BigInteger::operator-=(const BigInteger &other) {
	BigInteger tmp = -other;
	*this += tmp;
	return *this;
}

BigInteger& BigInteger::operator++() {
	*this = *this + BigInteger(1);
	return *this;
}

const BigInteger BigInteger::operator++(int) {
	BigInteger tmp = *this;
	++(*this);
	return tmp;
}

BigInteger &BigInteger::operator--() {
	*this = *this + BigInteger(-1);
	return *this;
}

const BigInteger BigInteger::operator--(int) {
	BigInteger tmp = *this;
	--(*this);
	return tmp;
}

bool BigInteger::operator==(const BigInteger &other) const {
	return (!(*this < other) && !(other < *this));
}

bool BigInteger::operator!=(const BigInteger &other) const {
	return !(*this == other);
}

bool BigInteger::operator>(const BigInteger &other) const {
	return other < *this;
}

bool BigInteger::operator<=(const BigInteger &other) const {
	return !(other < *this);
}

bool BigInteger::operator>=(const BigInteger &other) const {
	return !(*this < other);
}

std::ostream& operator<<(std::ostream &os, const BigInteger& bint) {
	os << bint.toString();
	return os;
}

std::istream& operator>>(std::istream &is, BigInteger& bint) {
	bint.big = {0};
	bool neg = false;
	std::string buff;
	is >> buff;
	if (buff[0] == '-') {
		neg = true;
		buff.erase(buff.begin());
	}
	BigInteger currMult = 1, mult = 1000000000;
	int64_t i;
	for (i = buff.size() - 9; i >= 0; i -= 9) {
		int val = std::stoi(buff.substr(i, i + 9));
		BigInteger tmp = val;
		tmp *= currMult;
		currMult *= mult;
		bint += tmp;
	}
	int val = std::stoi(buff.substr(std::max(i, 0ll), i + 9));
	BigInteger tmp = val;
	tmp *= currMult;
	bint += tmp;
	bint.positive = !neg;
	return is;
}

std::string BigInteger::toString() const {
	std::string res;
	BigInteger tmp = *this;
	const BigInteger ten(10);
	while (tmp) {
		res.insert(res.begin(), '0' + std::abs(static_cast<int>(tmp % ten)));
		tmp /= ten;
	}
	if (res.empty())
		res.insert(res.begin(), '0');
	else if (!positive)
		res.insert(res.begin(), '-');
	return res;
}

BigInteger &BigInteger::operator/=(const BigInteger &other) {
	BigInteger oth = other;
	oth.fix();
	fix();
	BigInteger part, final;
	part.big.clear();
	final.big.clear();
	bool initialSign = this->positive;
	for (int64_t i = parts() - 1; i >= 0; --i) { // getpart returns a new 'digit' (of 32-bits)
		uint64_t result = 0;
		part.big.insert(part.big.begin(), big[i]);
		bool sign = part.positive = oth.positive;
		while (sign == part.positive) {
			part -= oth;
			++result;
		}
		part += oth;
		--result;
		final.big.insert(final.big.begin(), static_cast<uint32_t>(result));
	}
	final.positive = ((initialSign && other.positive) || (!initialSign && !other.positive));
	final.fix();
	*this = final;
	return *this;
}

BigInteger &BigInteger::operator*=(const BigInteger &other) {
	BigInteger MAX = max;
	if (this->abs() <= MAX && other.abs() <= MAX) {
		uint64_t res = big[0];
		res *= other.big[0];
		big[0] = res % max;
		if (big.size() > 1)
			big[1] = res / max;
		else
			big.push_back(res / max);
		positive = ((positive && other.positive) || (!positive && !other.positive));
		return *this;
	}
	size_t n = std::max(parts(), other.parts());
	BigInteger a0, a1, b0, b1;
	a0.positive = a1.positive = positive;
	b0.positive = b1.positive = other.positive;
	a0.big = std::vector<uint32_t>(big.begin(), std::min(big.end(), big.begin() + n/2));
	a1.big = std::vector<uint32_t>(std::min(big.end(), big.begin() + n/2), big.end());
	b0.big = std::vector<uint32_t>(other.big.begin(), std::min(other.big.end(), other.big.begin() + n/2));
	b1.big = std::vector<uint32_t>(std::min(other.big.end(), other.big.begin() + n/2), other.big.end());
	if (a1.big.empty()) a1.big = {0};
	if (b1.big.empty()) b1.big = {0};
	if (a0.big.empty()) a0.big = {0};
	if (b0.big.empty()) b0.big = {0};
	BigInteger first = a0 * b0;
	BigInteger third = a1 * b1;
	BigInteger second = (a0 + a1)*(b0 + b1) - first - third;
	for (size_t i = 0; i < n/2; ++i) {
		third.big.insert(third.big.begin(), 0);
		third.big.insert(third.big.begin(), 0);
		second.big.insert(second.big.begin(), 0);
	}
	*this = first + second + third;
	return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger &other) {
	*this = *this - (*this / other) * other;
	return *this;
}

BigInteger BigInteger::operator*(const BigInteger &other) const {
	BigInteger tmp = *this;
	tmp *= other;
	return tmp;
}

BigInteger BigInteger::operator/(const BigInteger &other) const {
	BigInteger tmp = *this;
	tmp /= other;
	return tmp;
}

BigInteger BigInteger::operator%(const BigInteger &other) const {
	BigInteger tmp = *this;
	tmp %= other;
	return tmp;
}

int main() {
	BigInteger b1;
	BigInteger b2;
	std::cin >> b1 >> b2;
	BigInteger b3 = b1 - b2;
//	std::cout << b3;
	int a = 1;
	return 0;
}
