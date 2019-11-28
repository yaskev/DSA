#include <iostream>
#include <vector>
#include <array>
#include <limits>

const double INF = std::numeric_limits<double>::max();

template <typename T>
class Point {
public:
	Point(T x_, T y_, T z_, int index_) : x(x_), y(y_), z(z_), index(index_) {}
	Point(T x_, T y_, T z_, int index_, int prev_, int next_) : x(x_), y(y_), z(z_),
										index(index_), prev(prev_), next(next_) {}
	T x, y, z;
	int index = 0;
	int prev = -1;
	int next = -1;
};

template <typename T>
class Facet {
public:
	Facet(const Point<T>& p1, const Point<T>& p2, const Point<T>& p3) : n1(p1.index), n2(p2.index), n3(p3.index) {}
	void print() const { std::cout << "3 " << n1 << " " << n2 << " " << n3 << "\n"; }
	bool operator<(const Facet<T>& other) const {
		long long two = 1000;
		long long one = 100000000;
		return (one*n1 + two*n2 + n3) < (other.n1*one + other.n2*two + other.n3);
	}
private:
	int n1, n2, n3;
};

template <typename T>
class ConvexHull {
public:
	explicit ConvexHull(std::vector<Point<T>>);
	void print();
private:
	int mergeHulls(std::vector<int>& tmp, const std::vector<int>& left, int& u, int& v, int j);
	void updatePointers(int u, int v, int k, int mid, std::vector<int>& tmp);
	double crossProduct(int p, int q, int r);
	void findInitialBridge(int& u, int& v);
	void rotateAxis();
	double getTime(int p, int q, int r);
	void update(int pos);
	std::vector<int> buildHullRecursively(int start, int size);

	std::vector<Point<T>> points;
	std::vector<int> hull;
	size_t nil;
};

template <typename T>
double ConvexHull<T>::crossProduct(int p, int q, int r) {
	if (p == nil || q == nil || r == nil) return 1;
	return (points[q].x - points[p].x)*(points[r].y - points[p].y) -
			(points[r].x - points[p].x)*(points[q].y - points[p].y);
}

template <typename T>
void ConvexHull<T>::findInitialBridge(int &u, int &v) { // What if u goes left to infinity? v to +inf?
	while (true) {
		if (crossProduct(u, v, points[v].next) < 0) {
			v = points[v].next;
		} else if (crossProduct(points[u].prev, u, v) < 0) {
			u = points[u].prev;
		} else {
			break;
		}
	}
}

template <typename T>
std::vector<int> ConvexHull<T>::buildHullRecursively(int start, int size) {
	std::vector<int> tmp;
	if (size == 1) {
		points[start].next = nil;
		points[start].prev = nil;
		tmp.push_back(nil);
		return tmp;
	}
	int v = 0;
	int u = start + (size / 2 - 1);
	int mid = v = points[u].next;
	auto left = buildHullRecursively(start, size / 2);
	auto right = buildHullRecursively(mid, size - size / 2);
	left.insert(left.end(), right.begin(), right.end());
	findInitialBridge(u, v);
	int k = mergeHulls(tmp, left, u, v, left.size() - right.size());
	updatePointers(u, v, k, mid, tmp);

	if (start == 0 && size + 1 == points.size()) {
		hull = std::move(tmp);
		return hull;
	}
	return tmp;
}

template <typename T>
void ConvexHull<T>::rotateAxis() {

}

template <typename T>
void ConvexHull<T>::print() {
	std::cout << hull.size() << "\n";
	std::vector<Facet<double>> out;
	for (int v : hull) {
		// Here we should check the order
		if (v == nil) break;
		out.emplace_back(points[points[v].prev], points[v], points[points[v].next]);
		update(v);
	}
	std::sort(out.begin(), out.end());
	for (const auto& facet : out) {
		facet.print();
	}
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
	rotateAxis();
	std::sort(points.begin(), points.end(), [](const Point<T>& p1, const Point<T>& p2)
																{ return p1.x < p2.x; });
	points[0].next = 1;
	for (size_t i = 1; i < points.size(); ++i) { // Also add NULL
		points[i].prev = i - 1;
		points[i].next = i + 1;
	}
	points.push_back(NIL);
	nil = points.size() - 1;
	buildHullRecursively(0, points.size() - 1);
}

template<typename T>
int ConvexHull<T>::mergeHulls(std::vector<int> &tmp, const std::vector<int> &left, int &u, int &v, int j) {
	int i = 0;
	std::array<double, 6> time{};
	double oldtime = -INF, newtime = INF;
	while (true) {
		int mintime = 0;
		newtime = INF;
		time[0] = getTime(points[left[i]].prev, left[i], points[left[i]].next);
		time[1] = getTime(points[left[j]].prev, left[j], points[left[j]].next);
		time[2] = getTime(u, points[u].next, v);
		time[3] = getTime(points[u].prev, u, v);
		time[4] = getTime(u, points[v].prev, v);
		time[5] = getTime(u, v, points[v].next);
		for (size_t f = 0; f < time.size(); ++f) {
			if (time[f] < newtime && time[f] > oldtime) {
				mintime = f;
				newtime = time[mintime];
			}
		}
		if (newtime > INF / 2) break;
		switch (mintime) {
			case 0:
				if (points[left[i]].x < points[u].x)
					tmp.push_back(left[i]);
				update(left[i++]);
				break;
			case 1:
				if (points[j].x > points[v].x)
					tmp.push_back(left[j]);
				update(left[j++]);
				break;
			case 2:
				u = points[u].next;
				tmp.push_back(u);
				break;
			case 3:
				u = points[u].prev;
				tmp.push_back(u);
				break;
			case 4:
				v = points[v].prev;
				tmp.push_back(v);
				break;
			case 5:
				v = points[v].next;
				tmp.push_back(v);
				break;
			default:
				break;
		}
		oldtime = newtime;
	}
	tmp.push_back(nil);
	return static_cast<int>(tmp.size() - 1);
}

template<typename T>
void ConvexHull<T>::updatePointers(int u, int v, int k, int mid, std::vector<int>& tmp) {
	points[u].next = v;
	points[v].prev = u;
	for (k--; k >= 0; --k) {
		if (points[tmp[k]].x <= points[u].x || points[tmp[k]].x >= points[v].x) {
			update(tmp[k]);
			if (tmp[k] == u) {
				u = points[u].prev;
			} else if (tmp[k] == v) {
				v = points[v].next;
			}
		} else {
			points[u].next = tmp[k];
			points[tmp[k]].prev = u;
			points[v].prev = tmp[k];
			points[tmp[k]].next = v;
			if (points[tmp[k]].x < points[mid].x) {
				u = tmp[k];
			} else {
				v = tmp[k];
			}
		}
	}
}

template<typename T>
double ConvexHull<T>::getTime(int p, int q, int r) {
	if (p == nil || q == nil || r == nil) return INF;
	return ((points[q].x - points[p].x)*(points[r].z - points[p].z)
			- (points[r].x - points[p].x)*(points[q].z - points[p].z)) / crossProduct(p, q, r);
}

int main() {
	int tests_no = 0, points_no = 0;
	double x = 0, y = 0, z = 0;
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