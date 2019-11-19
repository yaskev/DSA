#include <iostream>
#include <array>
#include <cmath>
#include <algorithm>

template <typename T, uint32_t Dim>
class Point {
public:
	explicit Point(const std::array<T, Dim>& coords_) : coords(coords_) {}
	Point(const Point& p1, const Point& p2, bool left);
	double distance(const Point& other) const;
private:
	std::array<T, Dim> coords;
};

template <typename T, uint32_t Dim>
class Segment {
private:
	Point<T, Dim> start, end;
public:
	double distance(const Segment& other) const;
	double distance(const Point<T, Dim>& point) const;
	Segment(const Point<T, Dim>& p1, const Point<T, Dim>& p2) : start(p1), end(p2) {}
};

template<typename Figure, typename Point>
double distanceTernary(const Figure& f, Point left, Point right, double eps=1e-10) {
	double minValue = 1e308;
	double oldMinValue = 0;
	while (fabs(minValue - oldMinValue) > eps) {
		auto oldLeft = left;
		auto oldRight = right;
		oldMinValue = minValue;
		left = Point(oldLeft, oldRight, true);
		right = Point(oldLeft, oldRight, false);
		double distLeft = f.distance(left);
		double distRight = f.distance(right);
		if (distLeft < distRight) {
			left = oldLeft;
			minValue = distLeft;
		} else {
			right = oldRight;
			minValue = distRight;
		}
	}
	return minValue;
}

template <typename T, uint32_t Dim>
double Point<T, Dim>::distance(const Point &other) const {
	double squares = 0;
	for (uint32_t i = 0; i < Dim; ++i) {
		squares += (other.coords[i] - coords[i]) * (other.coords[i] - coords[i]);
	}
	return squares;
}

template <typename T, uint32_t Dim>
Point<T, Dim>::Point(const Point& p1, const Point& p2, bool left) {
	for (uint32_t i = 0; i < Dim; ++i) {
		double difference = p2.coords[i] - p1.coords[i];
		coords[i] = p1.coords[i] + (left ? difference / 3 : 2 * difference / 3);
	}
}

template <typename T, uint32_t Dim>
double Segment<T, Dim>::distance(const Segment& other) const {
	return sqrt(distanceTernary(other, start, end));
}

template <typename T, uint32_t Dim>
double Segment<T, Dim>::distance(const Point<T, Dim>& point) const {
	return distanceTernary(point, start, end);
}

int main() {
	std::array<double, 3> arr1{}, arr2{}, arr3{}, arr4{};
	for (int i = 0; i < 3; ++i) {
		std::cin >> arr1[i];
	}
	for (int i = 0; i < 3; ++i) {
		std::cin >> arr2[i];
	}
	for (int i = 0; i < 3; ++i) {
		std::cin >> arr3[i];
	}
	for (int i = 0; i < 3; ++i) {
		std::cin >> arr4[i];
	}
	Segment<double, 3> segment1((Point<double, 3>(arr1)), Point<double, 3>(arr2));
	Segment<double, 3> segment2((Point<double, 3>(arr3)), Point<double, 3>(arr4));

	std::cout << std::fixed << segment1.distance(segment2);

	return 0;
}
