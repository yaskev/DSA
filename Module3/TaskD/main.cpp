#include <iostream>
#include <vector>
#include <limits>
#include <fstream>
#include <algorithm>
#include <cmath>

constexpr double INF = std::numeric_limits<double>::max();
constexpr double EPS = 1e-8;

template <typename T>
class ConvexHull;

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
class Cmp {
public:
	bool operator()(const Point<T>* p1, const Point<T>* p2) {return cos(p1) > cos(p2);}
	explicit Cmp(const Point<T>* p) : base(p) {}
private:
	double cos(const Point<T>* p) {
		if (p == base) return 2;
		return (p->x - base->x) / sqrt((p->x - base->x)*(p->x - base->x) + (p->y - base->y)*(p->y - base->y));
	}
	const Point<T> *base;

};

template <typename T>
std::vector<int> build2DHull(std::vector<Point<T>*> points) { // Graham algorithm
	std::vector<int> res(points.size(), 0);
	auto minElem = std::min_element(points.begin(), points.end(),
			[](const Point<T>* p1, const Point<T>* p2){return fabs(p1->y - p2->y) < EPS ? p1->x < p2->x : p1->y < p2->y;});
	Cmp<T> cmp(*minElem);
	std::sort(points.begin(), points.end(), cmp);
	std::vector<Point<T>*> stack;
	stack.reserve(points.size());
	for (const auto p : points) {
		while (stack.size() > 1 && ConvexHull<T>::crossProduct(stack[stack.size() - 2], stack[stack.size() - 1], p) < 0) {
			stack.pop_back();
		}
		stack.push_back(p);
	}
	for (const auto p : stack) {
		res[p->index]++;
	}
	return res;
}

template <typename T>
class ConvexHull {
public:
	ConvexHull(std::vector<Point<T>*>, bool lower);
	std::vector<int> countEdges();
	static double crossProduct(const Point<T>*, const Point<T>*, const Point<T>*);
private:
	void updatePointers(Point<T>*, Point<T>*, Point<T>*, const std::vector<Point<T>*>&);
	void findInitialBridge(Point<T>*&, Point<T>*&);
	void rotateAxis(const std::vector<Point<T>*>&);
	double getTime(const Point<T>*, const Point<T>*, const Point<T>*);
	std::vector<Point<T>*> buildHullRecursively(Point<T>*, int);

	size_t pointNo;
	std::vector<Point<T>*> hull;
	static Point<T>* NIL;
	static Point<T> nilHolder;
};

template <typename T>
Point<T> ConvexHull<T>::nilHolder = Point<T>(INF, INF, INF, -1);
template <typename T>
Point<T>* ConvexHull<T>::NIL = &nilHolder;

template <typename T>
double ConvexHull<T>::crossProduct(const Point<T> *p, const Point<T> *q, const Point<T> *r) {
	if (p == NIL || q == NIL || r == NIL) return 1;
	return (q->x-p->x)*(r->y-p->y) - (r->x-p->x)*(q->y-p->y);
}

template <typename T>
double ConvexHull<T>::getTime(const Point<T> *p, const Point<T> *q, const Point<T> *r) {
	if (p == NIL || q == NIL || r == NIL) return INF;
	return ((q->x-p->x)*(r->z-p->z) - (r->x-p->x)*(q->z-p->z)) / crossProduct(p, q, r);
}

template <typename T>
ConvexHull<T>::ConvexHull(std::vector<Point<T>*> points, bool lower) {
	if (!lower) {
		for (auto p : points) {
			p->x *= -1;
			p->y *= -1;
			p->z *= -1;
		}
	}
	pointNo = points.size();
	rotateAxis(points);
	std::sort(points.begin(), points.end(), [](const Point<double>* p1,
											   const Point<double>* p2){return *p1 < *p2;});
	points[0]->next = points[1];
	points[pointNo - 1]->prev = points[pointNo - 2];
	points[pointNo - 1]->next = NIL;
	for (int i = 1; i + 1 < pointNo; ++i) {
		points[i]->next = points[i + 1];
		points[i]->prev = points[i - 1];
	}
	hull = buildHullRecursively(points[0], pointNo);
}

template<typename T>
std::vector<Point<T>*> ConvexHull<T>::buildHullRecursively(Point<T> *point, int n) {
	std::vector<Point<T>*> res;
	if (n == 1) {
		point->prev = point->next = NIL;
		res.push_back(NIL);
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
	res.push_back(NIL);
	return res;
}

template<typename T>
std::vector<int> ConvexHull<T>::countEdges() {
	std::vector<int> res(pointNo, 0);
	for (int i = 0; hull[i] != NIL; hull[i++]->update()) {
		res[hull[i]->prev->index]++;
		res[hull[i]->next->index]++;
		res[hull[i]->index]++;
	}
	return res;
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
void ConvexHull<T>::rotateAxis(const std::vector<Point<T>*>& points) {
	constexpr double alpha = 1e-7;
	double cosA = cos(alpha);
	double sinA = sin(alpha);
	for (auto p : points) {
		p->y = p->y * cosA - p->z * sinA;
		p->z = p->y * sinA + p->z * cosA;
		p->x = p->x * cosA + p->z * sinA;
		p->z = -p->x * sinA + p->z * cosA;
		p->x = p->x * cosA - p->y * sinA;
		p->y = p->x * sinA + p->y * cosA;
	}
}

int main() {
	std::ifstream fin("input.txt");
	double x = 0, y = 0;
	std::vector<Point<double>*> points;
	int counter = 0;
	while (fin >> x >> y) {
		points.push_back(new Point<double>(x, y, x*x + y*y, counter));
		++counter;
	}
	std::vector<int> infEdges = build2DHull(points);
	ConvexHull<double> lower(points, true);
	std::vector<int> edges = lower.countEdges();
	int finiteSections = 0;
	double totalEdgesSum = 0;
	for (int i = 0; i < counter; ++i) {
		if (infEdges[i] == 0) {
			finiteSections++;
			totalEdgesSum += edges[i];
		}
	}
	std::cout << std::fixed << (finiteSections == 0 ? 0 : totalEdgesSum / finiteSections) << std::endl;
	for (const auto& p : points) {
		delete p;
	}
	fin.close();
	return 0;
}
