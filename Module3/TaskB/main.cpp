#include <iostream>
#include <vector>
#include <limits>
#include <fstream>
#include <algorithm>
#include <cmath>

constexpr double INF = std::numeric_limits<double>::max();
constexpr double EPS = 1e-9;

template <typename T>
class Point {
public:
	Point() : x(0), y(0), z(0), index(-1), prev(nullptr), next(nullptr) {}
	Point(T x_, T y_, T z_, int index_) : x(x_), y(y_), z(z_),
										  index(index_), prev(nullptr), next(nullptr) {}
	Point(T x_, T y_, T z_, int index_, Point *prev_, Point *next_) : x(x_), y(y_), z(z_),
											index(index_), prev(prev_), next(next_) {}
	T x, y, z;
	int index = 0;
	Point *prev, *next;
	bool operator<(const Point<T>& other) const { return x < other.x; }
	void update();
};

template <typename T>
void Point<T>::update() {
	if (prev->next != this) { // Case of insertion
		next->prev = this;
		prev->next = this;
	}
	else { // Case of removal
		prev->next = next;
		next->prev = prev;
	}
}

template <typename T>
class Facet {
public:
	Facet(const Point<T>*, const Point<T>*, const Point<T>*);
	bool operator<(const Facet& other) const {
		long long one = 100000000;
		long long two = 10000;
		return n1*one + n2*two + n3 < (other.n1)*one + (other.n2)*two + (other.n3);
	}
	void print() const { std::cout << 3 << " " << n1 << " " << n2 << " " << n3 << "\n"; }
private:
	int n1, n2, n3;
};

template <typename T>
Facet<T>::Facet(const Point<T>* p1, const Point<T>* p2, const Point<T>* p3) {
	int tmp[3] = {p1->index, p2->index, p3->index};
	int minIndex = 0;
	for (int i = 1; i < 3; ++i) {
		if (tmp[i] < tmp[minIndex]) {
			minIndex = i;
		}
	}
	n1 = tmp[minIndex];
	n2 = tmp[(minIndex + 1) % 3];
	n3 = tmp[(minIndex + 2) % 3];
}

template <typename T>
class ConvexHull {
public:
	explicit ConvexHull(std::vector<Point<T>>&);
	~ConvexHull();
	void print();
private:
	void updatePointers(Point<T>*, Point<T>*, Point<T>*, const std::vector<Point<T>*>&);
	double crossProduct(const Point<T>*, const Point<T>*, const Point<T>*);
	void findInitialBridge(Point<T>*&, Point<T>*&);
	void rotateAxis(std::vector<Point<T>>&);
	double getTime(const Point<T>*, const Point<T>*, const Point<T>*);
	std::vector<Point<T>*> buildHullRecursively(Point<T>*, int);
	void toFacets(std::vector<Facet<T>>&);

	std::vector<Point<T>*> lower;
	std::vector<Point<T>*> upper;
	size_t pointNo;
	std::vector<Point<T>*> lowerHull;
	std::vector<Point<T>*> upperHull;
	static Point<T> NIL;
};

template <typename T>
Point<T> ConvexHull<T>::NIL = Point<T>(INF, INF, INF, -1);

template <typename T>
void ConvexHull<T>::print() {
	std::vector<Facet<double>> facets;
	toFacets(facets);
	std::sort(facets.begin(), facets.end());
	std::cout << facets.size() << std::endl;
	for (const auto& facet : facets) {
		facet.print();
	}
}

template <typename T>
double ConvexHull<T>::crossProduct(const Point<T> *p, const Point<T> *q, const Point<T> *r) {
	if (p == &NIL || q == &NIL || r == &NIL) return 1;
	return (q->x-p->x)*(r->y-p->y) - (r->x-p->x)*(q->y-p->y);
}

template <typename T>
double ConvexHull<T>::getTime(const Point<T> *p, const Point<T> *q, const Point<T> *r) {
	if (p == &NIL || q == &NIL || r == &NIL) return INF;
	return ((q->x-p->x)*(r->z-p->z) - (r->x-p->x)*(q->z-p->z)) / crossProduct(p, q, r);
}

template <typename T>
ConvexHull<T>::ConvexHull(std::vector<Point<T>>& points) {
	lower.reserve(points.size());
	upper.reserve(points.size());
	rotateAxis(points); // Rotate just once
	for (const auto& p : points) {
		lower.push_back(new Point<T>(p.x, p.y, p.z, p.index));
		upper.push_back(new Point<T>(-p.x, -p.y, -p.z, p.index));
	}
	std::sort(upper.begin(), upper.end(), [](const Point<T>* p1, const Point<T>* p2){return *p1 < *p2;});
	std::sort(lower.begin(), lower.end(), [](const Point<T>* p1, const Point<T>* p2){return *p1 < *p2;});
	pointNo = points.size();
	// Set initial pointers
	lower[0]->next = lower[1];
	lower[0]->prev = nullptr;
	lower[pointNo - 1]->prev = lower[pointNo - 2];
	lower[pointNo - 1]->next = &NIL;
	upper[0]->next = upper[1];
	upper[0]->prev = nullptr;
	upper[pointNo - 1]->prev = upper[pointNo - 2];
	upper[pointNo - 1]->next = &NIL;
	for (int i = 1; i + 1 < pointNo; ++i) {
		lower[i]->next = lower[i + 1];
		lower[i]->prev = lower[i - 1];
		upper[i]->next = upper[i + 1];
		upper[i]->prev = upper[i + 1];
	}
	lowerHull = buildHullRecursively(lower[0], pointNo);
	upperHull = buildHullRecursively(upper[0], pointNo);
}

template<typename T>
std::vector<Point<T>*> ConvexHull<T>::buildHullRecursively(Point<T> *point, int n) {
	std::vector<Point<T>*> res;
	if (n == 1) {
		point->prev = point->next = &NIL;
		res.push_back(&NIL);
		return res;
	}
	Point<T> *u = point;
	for (int i = 0; i < n / 2 - 1; ++i) {
		u = u->next;
	}
	Point<T> *v = u->next;
	Point<T> *mid = v;

	auto left = buildHullRecursively(point, n / 2);
	auto right = buildHullRecursively(mid, n - n / 2);
	findInitialBridge(u, v);

	int i = 0, j = 0;
	double oldTime = -INF;
	while (true) {
		double time[6], newTime = INF;
		int minIndex = -1;
		time[0] = getTime(left[i]->prev, left[i], left[i]->next);
		time[1] = getTime(right[j]->prev, right[j], right[j]->next);
		time[2] = getTime(u, u->next, v);
		time[3] = getTime(u->prev, u, v);
		time[4] = getTime(u, v->prev, v);
		time[5] = getTime(u, v, v->next);
		for (int l = 0; l < 6; ++l) {
			if (time[l] > oldTime && time[l] < newTime) {
				minIndex = l;
				newTime = time[l];
			}
		}
		if (newTime > INF / 2) break;
		switch (minIndex) {
			case 0:
				if (left[i]->x < u->x)
					res.push_back(left[i]);
				left[i++]->update();
				break;
			case 1:
				if (right[j]->x > v->x)
					res.push_back(right[j]);
				right[j++]->update();
				break;
			case 2:
				u = u->next;
				res.push_back(u);
				break;
			case 3:
				res.push_back(u);
				u = u->prev;
				break;
			case 4:
				v = v->prev;
				res.push_back(v);
				break;
			case 5:
				res.push_back(v);
				v = v->next;
				break;
			default:
				break;
		}
		oldTime = newTime;
	}
	updatePointers(u, v, mid, res);
	res.push_back(&NIL);
	return res;
}

template<typename T>
void ConvexHull<T>::toFacets(std::vector<Facet<T>>& facets) {
	for (int i = 0; lowerHull[i] != &NIL; lowerHull[i++]->update()) {
		if (crossProduct(lowerHull[i]->prev, lowerHull[i], lowerHull[i]->next) >= 0) {
			facets.emplace_back(lowerHull[i]->prev, lowerHull[i]->next, lowerHull[i]);
		} else {
			facets.emplace_back(lowerHull[i], lowerHull[i]->next, lowerHull[i]->prev);
		}
	}
	for (int i = 0; upperHull[i] != &NIL; upperHull[i++]->update()) {
		if (crossProduct(upperHull[i]->prev, upperHull[i], upperHull[i]->next) < 0) {
			facets.emplace_back(upperHull[i]->prev, upperHull[i]->next, upperHull[i]);
		} else {
			facets.emplace_back(upperHull[i], upperHull[i]->next, upperHull[i]->prev);
		}
	}
}

template<typename T>
void ConvexHull<T>::findInitialBridge(Point<T>*& u, Point<T>*& v) {
	while (true) {
		if (crossProduct(u, v, v->next) < 0)
			v = v->next;
		else if (crossProduct(u->prev, u, v) < 0)
			u = u->prev;
		else break;
	}
}

template<typename T>
void ConvexHull<T>::updatePointers(Point<T>* u, Point<T>* v, Point<T>* mid, const std::vector<Point<T>*>& res) {
	u->next = v;
	v->prev = u;
	for (int i = res.size() - 1; i >= 0; --i) {
		if (res[i]->x <= u->x || res[i]->x >= v->x) {
			res[i]->update();
			if (res[i] == u) {
				u = u->prev;
			} else if (res[i] == v) {
				v = v->next;
			}
		} else {
			u->next = res[i];
			res[i]->prev = u;
			v->prev = res[i];
			res[i]->next = v;
			if (res[i]->x < mid->x) {
				u = res[i];
			} else {
				v = res[i];
			}
		}
	}
}

template<typename T>
void ConvexHull<T>::rotateAxis(std::vector<Point<T>>& points) {
	constexpr double alpha = 1e-6;
	double cosA = cos(alpha);
	double sinA = sin(alpha);
	for (auto &p : points) {
		p.y = p.y * cosA - p.z * sinA;
		p.z = p.y * sinA + p.z * cosA;
		p.x = p.x * cosA + p.z * sinA;
		p.z = -p.x * sinA + p.z * cosA;
	}
	bool success = true;
	std::vector<Point<T>> tmp = points;
	// The best way to compare x-coordinates for equality that I managed to invent
	std::sort(tmp.begin(), tmp.end());
	for (size_t i = 1; i < tmp.size() && success; ++i) {
		if (std::abs(tmp[i].x - tmp[i-1].x) < EPS)
			success = false;
	}
	if (!success)
		rotateAxis(points);
}

template<typename T>
ConvexHull<T>::~ConvexHull() {
	for (auto p : lower)
		delete p;
	for (auto p : upper)
		delete p;
}

int main() {
	std::ifstream fin("../input.txt");
	int testsNo = 0;
	fin >> testsNo;
	for (int k = 0; k < testsNo; ++k) {
		int n = 0;
		double x = 0, y = 0, z = 0;
		fin >> n;
		std::vector<Point<double>> points;
		points.reserve(n);
		for (int i = 0; i < n; ++i) {
			fin >> x >> y >> z;
			points.emplace_back(x, y, z, i);
		}
		ConvexHull<double> hull(points);
		hull.print();
	}
	fin.close();
	return 0;
}
