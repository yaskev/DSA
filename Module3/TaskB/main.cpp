#include <iostream>
#include <vector>
#include <array>
#include <limits>

const double INF = std::numeric_limits<double>::max();

template <typename T>
class Point {
public:
	T x, y, z;
	int index = 0;
	int prev = -1;
	int next = -1;
};

template <typename T>
class ConvexHull {
public:
	explicit ConvexHull(std::vector<Point<T>>);
	void print();
private:
	void rotateAxis();
	void update(int pos);
	std::vector<int> buildHullRecursively(int start, int size);
	std::vector<Point<T>> points;
	std::vector<int> hull;
};

template <typename T>
std::vector<int> ConvexHull<T>::buildHullRecursively(int start, int size) {
	size_t last = points.size();
	std::vector<int> tmp;
	if (size == 1) {
		points[start].next = last;
		points[start].prev = last;
		tmp.push_back(last);
		return tmp;
	}
	int u = 0, v = 0, mid = 0;
	double time[6];
	double oldtime = 0, newtime = 0;
	int k = 0;


}

template <typename T>
void rotateAxis() {

}

template <typename T>
void ConvexHull<T>::print() {
//	std::cout << hull.size() << "\n";

}

template <typename T>
void ConvexHull<T>::update(int pos) {
	auto curr = points[pos];
	if (points[curr.prev].next == pos) { // Remove case
		points[curr.next].prev = curr.prev;
		points[curr.prev].next = curr.prev;
	} else { // Insert case
		points[curr.prev].next = pos;
		points[curr.next].prev = pos;
	}
}

template <typename T>
ConvexHull<T>::ConvexHull(std::vector<Point<T>> points_) {
	Point<T> NIL = {INF, INF, INF, -1, -1, -1};
	points = std::move(points_);
	points.push_back(NIL);
	rotateAxis();
	std::sort(points.begin(), points.end(), [](const Point<T>& p1, const Point<T>& p2)
	{ return p1.x < p2.x; });
	points[0].next = 1;
	points[points.size() - 1].prev = points.size() - 2;
	for (size_t i = 1; i + 1 < points.size(); ++i) {
		points[i].prev = i - 1;
		points[i].next = i + 1;
	}
	buildHullRecursively(0, points.size());
}

int main() {
	int tests_no = 0, points_no = 0;
	double x, y, z;
	std::cin >> tests_no;
	for (int i = 0; i < tests_no; ++i) {
		std::vector<Point<double>> hull;
		std::cin >> points_no;
		hull.reserve(points_no + 1); // Add one more place to accomodate NIL
		for (int j = 0; j < points_no; ++j) {
			std::cin >> x >> y >> z;
			hull.emplace_back(x, y, z, j);
		}
		ConvexHull<double> convHull(hull);
		convHull.print();
	}

	return 0;
}