#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>               // std::string
#include <stdexcept>            // std::runtime_error, std::logic_error
#include <cstdint>              // uint32_t, int32_t etc.
#include <memory>               // std::unique_ptr
#include <cassert>              // assert()

namespace sdl {

/// Ensures that SDL and its satellite libraries are properly initialized,
/// and deinitialized upon destruction. Can be nested.
class InitGuard {
public:
    InitGuard();
    ~InitGuard();
    InitGuard(InitGuard&) = delete;
    InitGuard& operator=(InitGuard&) = delete;
};

/// Returns the last SDL error message as a C++ string.
std::string get_error();

/// Returns the SDL timestamp (milliseconds since initialization).
uint64_t get_ticks();

using Event = SDL_Event;

//typedef SDL_Event Event;

//class Event;
//std::unique_ptr<Event> poll_event();

bool poll_event(SDL_Event& event);

const SDL_Color COLOR_WHITE = SDL_Color{ 255, 255, 255, 255 };
const SDL_Color COLOR_BLACK = SDL_Color{ 0, 0, 0, 255 };

class Size2d {
public:
    uint32_t w, h;

    Size2d() : w(0), h(0) {}
    Size2d(uint32_t w_, uint32_t h_) : w(w_), h(h_) {}
};

/// Minimum and maximum ranges of 2d coordinates.
/// (Please note this is different from Rect although the information
/// is the same; Rect uses a point and sizes (technically unsigned)
/// while Extents2d uses signed ranges.)
class Extents2d {
public:
    int32_t min_x, max_x, min_y, max_y;

    Extents2d() : min_x(0), max_x(0), min_y(0), max_y(0) {}
    Extents2d(int32_t min_x_, int32_t max_x_, int32_t min_y_, int32_t max_y_)
        : min_x(min_x_), max_x(max_x_), min_y(min_y_), max_y(max_y_)
        {
            if ((max_x < min_x) || (max_y < min_y)) {
                throw std::out_of_range("invalid Extents2d values (max < min)");
            }
        }
};

/// A point in 2d space with integer coordinates (on screen, surface etc.)
class Point2d {
public:
    int32_t x, y;

    Point2d() : x(0), y(0) {}
    Point2d(int32_t x_, int32_t y_) : x(x_), y(y_) {}
};

class Rect : public SDL_Rect {
    /* SDL_Rect has: int x, int y, int w, int h */
public:
    Rect(int x_, int y_, int w_, int h_)  { x = x_; y = y_; w = w_; h = h_; }
    Rect(Point2d topleft, int w_, int h_) { x = topleft.x; y = topleft.y; w = w_; h = h_; }
    Rect(int x_, int y_, Size2d extents)  { x = x_; y = y_; w = extents.w; h = extents.h; }
    Rect(Point2d topleft, Size2d extents) { x = topleft.x; y = topleft.y; w = extents.w; h = extents.h; }
    Rect(SDL_Rect &source) : SDL_Rect(source) {}

    // empty(), is_empty() are aliases
    bool empty() const { return SDL_RectEmpty(this); }
    bool is_empty() const { return SDL_RectEmpty(this); }

    // intersects(), has_intersection() are aliases
    bool intersects(Rect& other) const { return SDL_HasIntersection(this, &other); }
    bool has_intersection(Rect& other) const { return SDL_HasIntersection(this, &other); }
};

#if 0
class MouseMotionEvent;
class MouseButtonEvent;
class MouseWheelEvent;

class Event {
public:
    enum class Type {
        UNKNOWN = -1,
        EMPTY = 0,
        QUIT_REQUEST,
        MOUSE_MOTION,
        MOUSE_BUTTON,
        MOUSE_WHEEL,
    };
    Type type;
    explicit Event(Type type_) : type(type_) {}
    Event(Event const&) = delete;
    virtual MouseMotionEvent& motion() { throw std::logic_error("bad event type"); }
    virtual MouseButtonEvent& button() { throw std::logic_error("bad event type"); }
    virtual MouseWheelEvent& wheel() { throw std::logic_error("bad event type"); }
};

class UnknownEvent : public Event {
public:
    UnknownEvent() : Event(Type::UNKNOWN) {}
};

class EmptyEvent : public Event {
public:
    EmptyEvent() : Event(Type::EMPTY) {}
};

class QuitRequestEvent : public Event {
public:
    QuitRequestEvent() : Event(Type::QUIT_REQUEST) {}
};

class MouseMotionEvent : public Event {
public:
    Point2d position;
    MouseMotionEvent(Point2d position_)
        : Event(Type::MOUSE_MOTION), position(position_) {}
    virtual MouseMotionEvent& motion() override { return *this; }
};

class MouseButtonEvent : public Event {
public:
    Point2d position;
    uint32_t button_index;
    bool pressed;
    MouseButtonEvent(Point2d position_, uint32_t b, bool state_)
        : Event(Type::MOUSE_BUTTON), position(position_), button_index(b), pressed(state_) {}
    virtual MouseButtonEvent& button() override { return *this; }
};

class MouseWheelEvent : public Event {
public:
    Point2d position;
    Point2d amount;
    MouseWheelEvent(Point2d position_, Point2d amount_)
        : Event(Type::MOUSE_WHEEL), position(position_), amount(amount_) {}
    virtual MouseWheelEvent& wheel() override { return *this; }
};

std::unique_ptr<sdl::Event> event_from_sdl(SDL_Event& ev);
#endif

template<class T> class Wrapper {
protected:
    T* m_inner;
public:
    Wrapper() { m_inner = nullptr; }
    Wrapper(Wrapper<T> const&) = delete;
    Wrapper(Wrapper<T>&& orig) = default;
    Wrapper& operator=(Wrapper<T> const&) = delete;
    virtual ~Wrapper() {}
    T* peek() { return m_inner; }
};

class Surface : public Wrapper<SDL_Surface> {
public:
    explicit Surface(SDL_Surface* wrapped) { assert(wrapped); m_inner = wrapped; }
    operator SDL_Surface*() { assert(m_inner); return m_inner; }
    Size2d get_size() { assert(m_inner); return Size2d(m_inner->w, m_inner->h); }
};

class Texture : public Wrapper<SDL_Texture> {
public:
    explicit Texture(SDL_Texture* wrapped) { assert(wrapped); m_inner = wrapped; }
    operator SDL_Texture*() { assert(m_inner); return m_inner; }
    ~Texture() { if (m_inner) { SDL_DestroyTexture(m_inner); } }
    Size2d get_size();
};

class GlyphMetrics {
public:
    uint32_t codepoint = 0;
    Extents2d extents;
    int32_t advance = 0;
};

class Font : public Wrapper<TTF_Font> {
public:
    explicit Font(TTF_Font* wrapped) { assert(wrapped); m_inner = wrapped; }
    Font(std::string const& name, uint32_t pt_size);
    ~Font() { if (m_inner) { TTF_CloseFont(m_inner); } }
    void set_size(uint32_t pt_size);
    bool is_fixed_width()   { assert(m_inner); return TTF_FontFaceIsFixedWidth(m_inner); }
    uint32_t get_line_skip() { assert(m_inner); return TTF_FontLineSkip(m_inner); }
    uint32_t get_ascent()   { assert(m_inner); return TTF_FontAscent(m_inner); }
    uint32_t get_descent()  { assert(m_inner); return TTF_FontDescent(m_inner); }
    std::unique_ptr<sdl::Surface> render(std::string text, SDL_Color color);
    std::unique_ptr<sdl::Surface> render_wrapped(std::string text, SDL_Color color, uint32_t max_width);
    bool has_glyph(uint32_t codepoint);
    GlyphMetrics get_glyph_metrics(uint32_t codepoint);
    uint32_t get_space_width();
};

class Window;

class Renderer : public Wrapper<SDL_Renderer> {
public:
    explicit Renderer(SDL_Renderer* wrapped) { assert(wrapped); m_inner = wrapped; }
    explicit Renderer(Window& window);
    ~Renderer();
    Size2d get_output_size();
    std::unique_ptr<Texture> make_texture(uint32_t format, uint32_t access, Size2d size);
    std::unique_ptr<Texture> texture_from_surface(Surface& surface);
    void fill_rect(SDL_Rect rect, SDL_Color color);
    void copy_texture(Texture& tex, SDL_Rect target, SDL_Rect source);
    void put_texture(Texture& tex, SDL_Rect target);
    void present();
};

class Window : public Wrapper<SDL_Window> {
public:
    explicit Window(SDL_Window* wrapped) { assert(wrapped); m_inner = wrapped; }
    Window(std::string title, Size2d size);
    ~Window();
};

} // namespace sdl
