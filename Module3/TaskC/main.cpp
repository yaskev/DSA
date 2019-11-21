#include <iostream>
#include <array>
#include <utility>
#include <vector>
#include <cmath>

constexpr double EPS = 1e-8;
const double PI = atan(1) * 4;

template <typename T, uint32_t Dim>
class Point {
public:
	explicit Point(const std::array<T, Dim>& coords_) : coords(coords_) {}
	Point(const Point<T, Dim>& other, bool negate);
	Point(const Point& p1, const Point& p2);
	[[nodiscard]] double angle(const Point& other) const;
	[[nodiscard]] T get(size_t i) const { return coords[(i % Dim)]; }
private:
	std::array<T, Dim> coords;
};

template <typename T, uint32_t Dim>
Point<T, Dim>::Point(const Point &p1, const Point &p2) {
	for (uint32_t i = 0; i < Dim; ++i) {
		coords[i] = p1.coords[i] + p2.coords[i];
	}
}

template <typename T, uint32_t Dim>
double Point<T, Dim>::angle(const Point &other) const {
	double ang = atan2(other.get(1) - coords[1], other.get(0) - coords[0]);
	return ang > -EPS ? ang : ang + 2*PI;
}

template <typename T, uint32_t Dim>
Point<T, Dim>::Point(const Point<T, Dim>& other, bool negate) {
	for (uint32_t i = 0; i < Dim; ++i) {
		coords[i] = negate ? (-1) * other.coords[i] : other.coords[i];
	}
}

template <typename Pt>
class Polygon {
public:
	explicit Polygon(std::vector<Pt> points_) : points(std::move(points_)) {}
	Polygon(const Polygon<Pt>& other, bool negate);
	[[nodiscard]] int getLeast() const;
	[[nodiscard]] size_t card() const { return points.size(); }
	[[nodiscard]] Pt get(size_t i) const { return points[((i + points.size()) % points.size())]; }
private:
	std::vector<Pt> points;
};

template <typename Pt>
Polygon<Pt>::Polygon(const Polygon<Pt> &other, bool negate) {
	for (size_t i = 0; i < other.points.size(); ++i) {
		points.push_back(Pt(other.points[i], negate));
	}
}

template <typename Pt>
int Polygon<Pt>::getLeast() const {
	double minY = 1e300, minX = 1e300;
	int index = -1;
	for (size_t i = 0; i < points.size(); ++i) {
		if (points[i].get(1) - minY < -EPS) {
			minY = points[i].get(1);
		}
	}
	for (size_t i = 0; i < points.size(); ++i) {
		if (fabs(points[i].get(1) - minY) < EPS && points[i].get(0) - minX < -EPS) {
			minX = points[i].get(0);
			index = i;
		}
	}
	return index;
}

template <typename Pt>
Polygon<Pt> makeMinkowsiSum(const Polygon<Pt>& first, const Polygon<Pt>& second) {
	std::vector<Pt> sum;
	int leastIndex1 = first.getLeast();
	int leastIndex2 = second.getLeast();
	int i = 0, j = 0;
	while (i < first.card() && j < second.card()) {
		sum.push_back(Pt(first.get(leastIndex1 - i), second.get(leastIndex2 - j)));
		if (first.get(leastIndex1 - i).angle(first.get(leastIndex1 - i - 1))
			< second.get(leastIndex2 - j).angle(second.get(leastIndex2 - j - 1))) {
			++i;
		} else {
			++j;
		}
	}
	while (i < first.card())  {
		sum.push_back(Pt(first.get(leastIndex1 - i), second.get(leastIndex2 - j)));
		++i;
	}
	while (j < second.card()) {
		sum.push_back(Pt(first.get(leastIndex1 - i), second.get(leastIndex2 - j)));
		++j;
	}

	return Polygon<Pt>(sum);
}

template <typename Pt>
bool zeroInside(const Polygon<Pt>& polygon) {
	bool res = true;
	for (size_t i = 0; i < polygon.card(); ++i) {
		const auto p1 = polygon.get(i);
		const auto p2 = polygon.get(i + 1);
		if (p1.get(0) * p2.get(1) - p1.get(1) * p2.get(0) < EPS) {
			res = false;
			break;
		}
	}
	return res;
}

template <typename Pt>
bool do_intersect(const Polygon<Pt>& p1, const Polygon<Pt>& p2) {
	Polygon<Pt> negated = Polygon<Pt>(p1, true); // Negate the polygon
	Polygon<Pt> sum = makeMinkowsiSum(negated, p2);
	return zeroInside(sum);
}

int main() {
	std::vector<Point<double, 2>> polygon1;
	std::vector<Point<double, 2>> polygon2;
	int n = 0;
	double x, y;
	std::cin >> n;
	for (int i = 0; i < n; ++i) {
		std::cin >> x >> y;
		polygon1.emplace_back(std::array<double,  2>{x, y});
	}
	std::cin >> n;
	for (int i = 0; i < n; ++i) {
		std::cin >> x >> y;
		polygon2.emplace_back(std::array<double, 2>{x, y});
	}
	std::cout << (do_intersect(Polygon<Point<double, 2>>(polygon1),
			Polygon<Point<double, 2>>(polygon2)) ? "YES"  : "NO");
	return 0;
}
