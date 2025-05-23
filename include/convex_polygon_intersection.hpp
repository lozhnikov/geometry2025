#include <iostream>
#include <vector>
#include <cmath>
struct Point {
    double x, y;
    Point operator-(const Point& p) const {
        return {x - p.x, y - p.y};
    }
    double cross(const Point& p) const {
        return x * p.y - y * p.x;
    }
};
bool isInside(const Point& p, const Point& a, const Point& b) {
    return ((b - a).cross(p - a)) >= 0;
}
Point intersection(const Point& a1, const Point& a2, const Point& b1, const Point& b2) {
    Point da = a2 - a1;
    Point db = b2 - b1;
    Point dp = a1 - b1;
    double dap_cross_db = da.cross(db);
    double t = db.cross(dp) / dap_cross_db;
    return {a1.x + t * da.x, a1.y + t * da.y};
}
bool isSamePoint(const Point& a, const Point& b, double eps = 1e-9) {
    return std::fabs(a.x - b.x) < eps && std::fabs(a.y - b.y) < eps;
}
std::vector<Point> Process(const std::vector<Point>& Polygon1, const std::vector<Point>& Polygon2) {
    std::vector<Point> output = Polygon1;
    for (size_t i = 0; i < Polygon2.size(); ++i) {
        Point A = Polygon2[i];
        Point B = Polygon2[(i + 1) % Polygon2.size()];
        std::vector<Point> input = output;
        output.clear();
        for (size_t j = 0; j < input.size(); ++j) {
            Point P = input[j];
            Point Q = input[(j + 1) % input.size()];
            bool P_inside = isInside(P, A, B);
            bool Q_inside = isInside(Q, A, B);
            if (P_inside && Q_inside) {
                output.push_back(Q);
            } else if (P_inside && !Q_inside) {
                output.push_back(intersection(P, Q, A, B));
            } else if (!P_inside && Q_inside) {
                output.push_back(intersection(P, Q, A, B));
                output.push_back(Q);
            }
        }
    }
    std::vector<Point> unique;
    for (const auto& p : output) {
        bool exists = false;
        for (const auto& q : unique) {
            if (isSamePoint(p, q)) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            unique.push_back(p);
        }
    }
    return unique;
}