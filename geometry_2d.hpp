#pragma once

#include <SDL2/SDL.h>

class Point2d {
public:
    int x = 0;
    int y = 0;
    Point2d() {}
    Point2d(int x, int y) : x(x), y(y) {}
};

class Size2d {
public:
    unsigned w = 0;
    unsigned h = 0;
    Size2d() {}
    Size2d(unsigned w, unsigned h) : w(w), h(h) {}
};

class Rect2d {
public:
    int x = 0;
    int y = 0;
    unsigned w = 0;
    unsigned h = 0;

    Rect2d() {}
    Rect2d(int x, int y, unsigned w, unsigned h) : x(x), y(y), w(w), h(h) {}
    Rect2d(Point2d top_left, Size2d size) : x(top_left.x), y(top_left.y), w(size.w), h(size.h) {}
    Rect2d(Rect2d const&) = default;
    Rect2d& operator=(Rect2d const&) = default;

    Point2d top_left() const { return Point2d(x, y); }
    Point2d top_right() const { return Point2d(x + w, y); }
    Point2d bottom_left() const { return Point2d(x, y + h); }
    Point2d bottom_right() const { return Point2d(x + w, y + h); }

    /// Returns true if the rectangle is empty, i.e. one or both dimensions are 0.
    /// For an empty rectangle, contains() returns false for any point,
    /// and drawing such a rectangle should have no effect.
    bool empty() const {
        return (w == 0 || h == 0);
    }

    /// Returns true if the rectangle contains the given point, false otherwise.
    /// If the rectangle is empty, contains() always returns false.
    /// For a non-empty rectangle, the top left point is always inside
    /// and the bottom right point is never inside.
    bool contains(Point2d p) const {
        return (p.x >= x && p.x < x+w && p.y >= y && p.y < y+h);
    }
};

// guard that Rect and SDL_Rect are really compatible
static_assert(sizeof(Rect2d) == sizeof(SDL_Rect));
static_assert(sizeof(Rect2d*) == sizeof(SDL_Rect*));
