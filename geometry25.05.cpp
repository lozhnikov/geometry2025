#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
#include <set>
#include <limits>
#include <cmath>

using namespace std;

enum Direction {LEFT_TO_RIGHT, RIGHT_TO_LEFT};
enum EventType {START_TYPE, BEND_TYPE, END_TYPE};
enum ElementType {ACTIVE_EDGE, ACTIVE_POINT};
enum Rotation {CLOCKWISE = 0, COUNTER_CLOCKWISE = 1};

class Edge;
class Vertex;
class Polygon;
class ActiveElement;
class ActiveEdge;
class ActivePoint;

Direction sweepdirection;
double curx;
EventType curtype;


struct Point {
    double x, y;
    Point(double x = 0.0, double y = 0.0) : x(x), y(y) {}
    Point operator-(const Point& p) const { return Point(x - p.x, y - p.y); }
    double length2() const { return x * x + y * y; }
};

class Edge {
private:
    Point p1_, p2_;

public:
    Edge(const Point& p1, const Point& p2) : p1_(p1), p2_(p2) {}
    double y(double x) const {
        if (p1_.x == p2_.x) return p1_.y;
        return p1_.y + (p2_.y - p1_.y) * (x - p1_.x) / (p2_.x - p1_.x);
    }

    double slope() const {
        if (p1_.x == p2_.x) return std::numeric_limits<double>::max();
        return (p2_.y - p1_.y) / (p2_.x - p1_.x);
    }
};

class Vertex {
private:
    Point p_;
    Vertex* cw_;
    Vertex* ccw_;
    Polygon* polygon_;
public:
    enum Classify {LEFT, RIGHT, BEYOND, BEHIND, BETWEEN};
    Vertex(const Point& p, Polygon* poly = nullptr)
        : p_(p), cw_(nullptr), ccw_(nullptr), polygon_(poly) {}

    const Point& point() const { return p_; }
    double x() const { return p_.x; }
    double y() const { return p_.y; }

    Vertex* cw() const { return cw_; }
    Vertex* ccw() const { return ccw_; }
    void set_cw(Vertex* v) { cw_ = v; }
    void set_ccw(Vertex* v) { ccw_ = v; }

    Vertex* split(Vertex* w) {
        Vertex* wp = new Vertex(w->point(), polygon_);
        wp->set_ccw(this);
        wp->set_cw(w->cw());
        w->cw()->set_ccw(wp);
        w->set_cw(wp);
        return wp;
    }

     Classify classify(const Vertex& u, const Vertex& v) const {
        Point a = v.p_ - u.p_;
        Point b = p_ - u.p_;
        double sa = a.x * b.y - b.x * a.y;
        if (sa > 0.0) return LEFT;
        if (sa < 0.0) return RIGHT;
        if (a.x * b.x < 0.0 || a.y * b.y < 0.0) return BEHIND;
        if (a.length2() < b.length2()) return BEYOND;
        return BETWEEN;
    }

    Vertex* neighbor(int rotation) const {
        return (rotation == CLOCKWISE) ? cw_ : ccw_;
    }
    Polygon* getPolygon() const { return polygon_; }
};

class Polygon {
private:
    Vertex* head_;
    int size_;

public:
    Polygon() : head_(nullptr), size_(0) {}

    void addVertex(const Point& p) {
        Vertex* v = new Vertex(p, this);
        if (!head_) {
            head_ = v;
            v->set_cw(v);
            v->set_ccw(v);
        } else {
            Vertex* tail = head_->ccw();
            v->set_cw(head_);
            v->set_ccw(tail);
            tail->set_cw(v);
            head_->set_ccw(v);
        }
        size_++;
    }

    Vertex* v() const { return head_; }
    void setV(Vertex* v) { head_ = v; }
    int size() const { return size_; }

    void advance(int rotation) {
        head_ = (rotation == CLOCKWISE) ? head_->cw() : head_->ccw();
    }

    bool isConvex(Vertex* v) const {
        Vertex* u = v->ccw();
        Vertex* w = v->cw();
        Vertex::Classify c = w->classify(*u, *v);
        return (c == Vertex::RIGHT || c == Vertex::BEYOND);
    }

    static Polygon* fromVertex(Vertex* v) {
        Polygon* poly = new Polygon();
        poly->addVertex(v->point());
        return poly;
    }
};

class ActiveElement {
public:
    ElementType type;
    ActiveElement(ElementType t) : type(t) {}
    virtual double y() const = 0;
    virtual double slope() const { return 0.0; }
    virtual Edge edge() const = 0;
    virtual ~ActiveElement() = default;
};

class ActiveEdge : public ActiveElement {
public:
    Vertex *v, *w;
    int rotation;

    ActiveEdge(Vertex* _v, int _r, Vertex* _w)
        : ActiveElement(ACTIVE_EDGE), v(_v), rotation(_r), w(_w) {}

    Edge edge() const override {
        return Edge(v->point(), v->cw()->point());
    }

    double y() const override {
        return edge().y(curx);
    }

    double slope() const override {
        return edge().slope();
    }
};

class ActivePoint : public ActiveElement {
public:
    Point p;
    ActivePoint(const Point& _p)
        : ActiveElement(ACTIVE_POINT), p(_p) {}

    Edge edge() const override {
        return Edge(p, p);
    }

    double y() const override {
        return p.y;
    }
};

int leftToRightCmp(Vertex* a, Vertex* b) {
    if (a->x() < b->x()) return -1;
    else if (a->x() > b->x()) return 1;
    return 0;
}

int rightToLeftCmp(Vertex* a, Vertex* b) {
    if (a->x() > b->x()) return -1;
    else if (a->x() < b->x()) return 1;
    return 0;
}

int activeElementCmp(ActiveElement* a, ActiveElement* b) {
    double ya = a->y();
    double yb = b->y();
    if (ya < yb) return -1;
    if (ya > yb) return 1;

    if (a->type == ACTIVE_POINT && b->type == ACTIVE_POINT) return 0;
    if (a->type == ACTIVE_POINT) return -1;
    if (b->type == ACTIVE_POINT) return 1;

    double ma = a->slope();
    double mb = b->slope();
    int rval = (sweepdirection == LEFT_TO_RIGHT && curtype == START_TYPE) ||
               (sweepdirection == RIGHT_TO_LEFT && curtype == END_TYPE) ? -1 : 1;
    if (ma < mb) return rval;
    if (ma > mb) return -rval;
    return 0;
}

using Sweepline = std::set<ActiveElement*, decltype(&activeElementCmp)>;

void startTransition(Vertex* v, Sweepline& sweepline) {
    ActivePoint tmp(v->point());
    auto it = sweepline.upper_bound(&tmp);
    ActiveEdge* a = dynamic_cast<ActiveEdge*>(*std::prev(it));
    Vertex* w = a->w;

    if (!v->getPolygon()->isConvex(v)) {
        Vertex* wp = v->split(w);
        sweepline.insert(new ActiveEdge(wp->cw(), CLOCKWISE, wp->cw()));
        sweepline.insert(new ActiveEdge(v->ccw(), COUNTER_CLOCKWISE, v));
        a->w = (sweepdirection == LEFT_TO_RIGHT) ? wp->ccw() : v;
    } else {
        sweepline.insert(new ActiveEdge(v->ccw(), COUNTER_CLOCKWISE, v));
        sweepline.insert(new ActiveEdge(v, CLOCKWISE, v));
        a->w = v;
    }
}

void bendTransition(Vertex* v, Sweepline& sweepline) {
    ActivePoint tmp(v->point());
    auto it = sweepline.upper_bound(&tmp);
    ActiveEdge* a = dynamic_cast<ActiveEdge*>(*std::prev(it));
    ActiveEdge* b = dynamic_cast<ActiveEdge*>(*it);

    a->w = v;
    b->w = v;
    b->v = b->v->neighbor(b->rotation);
}

void endTransition(Vertex* v, Sweepline& sweepline, std::list<Polygon*>& polys) {
    ActivePoint tmp(v->point());
    auto it = sweepline.upper_bound(&tmp);
    ActiveEdge* a = dynamic_cast<ActiveEdge*>(*std::prev(it));
    ActiveEdge* b = dynamic_cast<ActiveEdge*>(*it);
    ActiveEdge* c = dynamic_cast<ActiveEdge*>(*std::next(it));

    if (v->getPolygon()->isConvex(v)) {
        polys.push_back(Polygon::fromVertex(v));
    } else {
        a->w = v;
    }

    sweepline.erase(b);
    sweepline.erase(c);
}

Sweepline buildSweepline() {
    Sweepline sweepline(activeElementCmp);
    sweepline.insert(new ActivePoint(Point(0.0, -numeric_limits<double>::max())));
    return sweepline;
}


Vertex** buildSchedule(Polygon& p, int (*cmp)(Vertex*, Vertex*)) {
    vector<Vertex*> schedule(p.size());
    for (int i = 0; i < p.size(); ++i) {
        schedule[i] = p.v();
        p.advance(CLOCKWISE);
    }
    sort(schedule.begin(), schedule.end(), [cmp](Vertex* a, Vertex* b) {
        return cmp(a, b) < 0;
    });
    Vertex** result = new Vertex*[schedule.size()];
    std::copy(schedule.begin(), schedule.end(), result);
    return result;
}

EventType typeEvent(Vertex* v, int (*cmp)(Vertex*, Vertex*)) {
    Vertex* prev = v->ccw();
    Vertex* next = v->cw();
    int cmpPrev = cmp(prev, v);
    int cmpNext = cmp(next, v);
    if (cmpPrev <= 0 && cmpNext <= 0) return END_TYPE;
    else if (cmpPrev > 0 && cmpNext > 0) return START_TYPE;
    else return BEND_TYPE;
}

void semiregularize(Polygon& p, Direction direction, list<Polygon*>& polys) {
    sweepdirection = direction;
    int (*cmp)(Vertex*, Vertex*);
    cmp = (direction == LEFT_TO_RIGHT) ? leftToRightCmp : rightToLeftCmp;
    Vertex** schedule = buildSchedule(p, cmp);

    Sweepline sweepline = buildSweepline();
    for (int i = 0; i < p.size(); ++i) {
        Vertex* v = schedule[i];
        curx = v->x();
        EventType event = typeEvent(v, cmp);
        switch (event) {
            case START_TYPE:
                startTransition(v, sweepline);
                break;
            case BEND_TYPE:
                bendTransition(v, sweepline);
                break;
            case END_TYPE:
                endTransition(v, sweepline, polys);
                break;
        }
        p.setV(static_cast<Vertex*>(nullptr));
    }
    delete[] schedule;
}

list<Polygon*> regularize(Polygon& p) {
    list<Polygon*> polys1;
    semiregularize(p, LEFT_TO_RIGHT, polys1);

    list<Polygon*> polys2;
    while (!polys1.empty()) {
        Polygon* q = polys1.back();
        polys1.pop_back();
        semiregularize(*q, RIGHT_TO_LEFT, polys2);
        delete q;
    }
    return polys2;
}

void TestPolygonConvexity() {
    Polygon poly;
    poly.addVertex(Point(0, 0));
    poly.addVertex(Point(2, 2));
    poly.addVertex(Point(1, 3));
    poly.addVertex(Point(3, 5));
    poly.addVertex(Point(4, 4));
    poly.addVertex(Point(5, 4));
    poly.addVertex(Point(6, 1));

    bool allTestsPassed = true;

    Vertex* v = poly.v();
    for (int i = 0; i < poly.size(); ++i) {
        bool isConvex = poly.isConvex(v);
        std::cout << "Vertex (" << v->x() << ", " << v->y() << ") is "
                  << (isConvex ? "convex" : "not convex") << std::endl;

        bool expectedConvex;
        if (i == 0) expectedConvex = true;
        else if (i == 1) expectedConvex = false;
        else if (i == 2) expectedConvex = true;
        else if (i == 3) expectedConvex = true;
        else if (i == 4) expectedConvex = false;
        else if (i == 5) expectedConvex = true;
        else if (i == 6) expectedConvex = true;

        if (isConvex != expectedConvex) {
            std::cerr << "ERROR: Vertex (" << v->x() << ", " << v->y()
                      << ") should be " << (expectedConvex ? "convex" : "not convex")
                      << ", but got " << (isConvex ? "convex" : "not convex") << std::endl;
            allTestsPassed = false;
        }

        v = v->cw();
    }

    if (allTestsPassed) {
        std::cout << "All convexity tests passed successfully!" << std::endl;
    } else {
        std::cerr << "Some convexity tests failed!" << std::endl;
    }
}

int main() {
    TestPolygonConvexity();
    return 0;
}
