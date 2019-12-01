#include <iostream>
#include <vector>
#include <array>
#include <limits>
#include <fstream>

constexpr double INF = std::numeric_limits<double>::max();

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
class ConvexHull {
public:
	explicit ConvexHull(std::vector<Point<T>*>);
	~ConvexHull() { delete[] Hull; }
	void print();
private:
//	int mergeHulls(std::vector<int>& tmp, const std::vector<int>& left, int& u, int& v, int j);
	void updatePointers(Point<T>*, Point<T>*, Point<T>*, const std::vector<Point<T>*>&);
	double crossProduct(const Point<T>*, const Point<T>*, const Point<T>*);
	void findInitialBridge(Point<T>*&, Point<T>*&);
//	void rotateAxis();
	double getTime(const Point<T>*, const Point<T>*, const Point<T>*);
	std::vector<Point<T>*> buildHullRecursively(Point<T>*, int, Point<T>**, Point<T>**);

	Point<T> **Hull;
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
ConvexHull<T>::ConvexHull(std::vector<Point<T>*> points) {
	pointNo = points.size();
//	rotateAxis();
	std::sort(points.begin(), points.end(), [](const Point<double>* p1,
											   const Point<double>* p2){return *p1 < *p2;});
	points[0]->next = points[1];
	points[pointNo - 1]->prev = points[pointNo - 2];
	points[pointNo - 1]->next = NIL;
	for (int i = 1; i + 1 < pointNo; ++i) {
		points[i]->next = points[i + 1];
		points[i]->prev = points[i - 1];
	}
	Hull = new Point<double> *[2*pointNo];
	auto **tmp = new Point<double> *[2*pointNo];
	hull = buildHullRecursively(points[0], pointNo, Hull, tmp);
	delete[] tmp;
}

template<typename T>
std::vector<Point<T>*> ConvexHull<T>::buildHullRecursively(Point<T> *point, int n, Point<T> **A, Point<T> **B) {
	std::vector<Point<T>*> res;
	if (n == 1) {
		point->prev = point->next = NIL;
		res.push_back(NIL);
		return res;
	}
	Point<T> *u = point;
	std::advance(u, n / 2 - 1);
	Point<T> *v = u->next;
	Point<T> *mid = v;

	auto left = buildHullRecursively(point, n / 2, B, A);
	auto right = buildHullRecursively(mid, n - n / 2, B + n / 2 * 2, A + n / 2 * 2);
	findInitialBridge(u, v);

	int i = 0, k = 0, j = 0;
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
void ConvexHull<T>::print() {
	for (int i = 0; hull[i] != NIL; hull[i++]->update()) {
		std::cout << hull[i]->prev->index << " " << hull[i]->index << " "
				  << hull[i]->next->index << "\n";
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
	for (auto elem = res.rbegin(); elem != res.rend(); ++elem) {
		if ((*(elem))->x <= u->x || (*(elem))->x >= v->x) {
			(*(elem))->update();
			if ((*(elem)) == u) {
				u = u->prev;
			} else if ((*(elem)) == v) {
				v = v->next;
			}
		} else {
			u->next = (*(elem));
			(*(elem))->prev = u;
			v->prev = (*(elem));
			(*(elem))->next = v;
			if ((*(elem))->x < mid->x) {
				u = (*(elem));
			} else {
				v = (*(elem));
			}
		}
	}
}

int main() {
	std::ifstream fin("../input.txt");
	int n = 0;
	double x, y, z;
	fin >> n;
	std::vector<Point<double>*> points;
	points.reserve(n);
	for (int i = 0; i < n; ++i) {
		fin >> x >> y >> z;
		points.push_back(new Point<double>(x, y, z, i));
	}
	ConvexHull<double> convexHull(points);
	convexHull.print();
	for (const auto& p : points) {
		delete p;
	}
	fin.close();
	return 0;
}