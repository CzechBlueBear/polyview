#include "sdl_wrapper.hpp"
#include <SDL2/SDL_image.h>

sdl::InitGuard::InitGuard() {
    if (0 != SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_AUDIO|SDL_INIT_EVENTS)) {
        throw std::runtime_error("SDL_Init() failed: " + sdl::get_error());
    }
    if (IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG) != (IMG_INIT_JPG|IMG_INIT_PNG)) {
        throw std::runtime_error("IMG_Init() failed: " + std::string(IMG_GetError()));
    }
    if (0 != TTF_Init()) {
        throw std::runtime_error("TTF_Init() failed: " + std::string(TTF_GetError()));
    }
}

sdl::InitGuard::~InitGuard() {
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

std::string sdl::get_error() {
    return std::string(SDL_GetError());
}

uint64_t sdl::get_ticks()
{
    return SDL_GetTicks();
}

const sdl::Color sdl::Color::WHITE = sdl::Color(255, 255, 255);
const sdl::Color sdl::Color::BLACK = sdl::Color(0, 0, 0);

bool sdl::poll_event(SDL_Event& event) {
    return !!SDL_PollEvent(&event);
}

// --- sdl::Texture ---

sdl::Size2d sdl::Texture::get_size()
{
    if (!m_inner) {
        return Size2d(0, 0);
    }
    int w, h;
    if (0 != SDL_QueryTexture(m_inner, nullptr, nullptr, &w, &h)) {
        throw std::runtime_error("SDL_QueryTexture() failed: " + sdl::get_error());
    }
    return Size2d(w, h);
}

// --- sdl::Font ---

sdl::Font::Font(std::string const& name, uint32_t pt_size)
{
    m_inner = TTF_OpenFont(name.c_str(), pt_size);
    if (!m_inner) {
        throw std::runtime_error("TTF_OpenFont() failed: " + std::string(TTF_GetError()));
    }
}

void sdl::Font::set_size(uint32_t pt_size)
{
    assert(m_inner);
    TTF_SetFontSize(m_inner, pt_size);
}

std::unique_ptr<sdl::Surface> sdl::Font::render(std::string text, SDL_Color color)
{
    assert(m_inner);
    SDL_Surface* surf = TTF_RenderUTF8_Blended(m_inner, text.c_str(), color);
    if (!surf) {
        throw std::runtime_error("TTF_RenderUTF8_Blended() failed: " + std::string(TTF_GetError()));
    }
    return std::make_unique<sdl::Surface>(surf);
}

std::unique_ptr<sdl::Surface> sdl::Font::render_wrapped(std::string text, SDL_Color color, uint32_t max_width)
{
    assert(m_inner);
    SDL_Surface* surf = TTF_RenderUTF8_Blended_Wrapped(m_inner, text.c_str(), color, max_width);
    if (!surf) {
        throw std::runtime_error("TTF_RenderUTF8_Blended_Wrapped() failed: " + std::string(TTF_GetError()));
    }
    return std::make_unique<sdl::Surface>(surf);
}

bool sdl::Font::has_glyph(uint32_t codepoint)
{
    assert(m_inner);
    return !!TTF_GlyphIsProvided32(m_inner, codepoint);
}

sdl::GlyphMetrics sdl::Font::get_glyph_metrics(uint32_t codepoint)
{
    assert(m_inner);
    int minx, miny, maxx, maxy, advance;
    if (0 != TTF_GlyphMetrics32(m_inner, codepoint, &minx, &maxx, &miny, &maxy, &advance)) {
        throw std::runtime_error("TTF_GlyphMetrics32() failed: " + std::string(TTF_GetError()));
    }
    return sdl::GlyphMetrics {
        .codepoint = codepoint,
        .extents = Extents2d(minx, miny, maxx, maxy),
        .advance = advance
    };
}

uint32_t sdl::Font::get_space_width()
{
    assert(m_inner);
    int minx, miny, maxx, maxy, advance;
    if (0 != TTF_GlyphMetrics32(m_inner, ' ', &minx, &maxx, &miny, &maxy, &advance)) {
        throw std::runtime_error("TTF_GlyphMetrics32() failed: " + std::string(TTF_GetError()));
    }
    return advance;
}

// --- sdl::Renderer ---

sdl::Renderer::Renderer(sdl::Window& window)
{
    m_inner = SDL_CreateRenderer(window.peek(), -1, SDL_RENDERER_ACCELERATED);
    if (!m_inner) {
        throw std::runtime_error("SDL_CreateRenderer() failed: " + sdl::get_error());
    }
}

sdl::Renderer::~Renderer()
{
    if (m_inner) {
        SDL_DestroyRenderer(m_inner);
    }
}

sdl::Size2d sdl::Renderer::get_output_size()
{
    int width, height;
    SDL_GetRendererOutputSize(m_inner, &width, &height);
    return Size2d(width, height);
}

std::unique_ptr<sdl::Texture> sdl::Renderer::make_texture(uint32_t format, uint32_t access, Size2d size)
{
    auto tex = SDL_CreateTexture(m_inner, format, access, size.w, size.h);
    if (!tex) {
        throw std::runtime_error("SDL_CreateTexture() failed: " + sdl::get_error());
    }
    return std::make_unique<sdl::Texture>(tex);
}

std::unique_ptr<sdl::Texture> sdl::Renderer::texture_from_surface(sdl::Surface& surface)
{
    auto tex = SDL_CreateTextureFromSurface(m_inner, surface);
    if (!tex) {
        throw std::runtime_error("SDL_CreateTextureFromSurface() failed: " + sdl::get_error());
    }
    return std::make_unique<sdl::Texture>(tex);
}

void sdl::Renderer::fill_rect(SDL_Rect rect, SDL_Color color)
{
    SDL_SetRenderDrawColor(m_inner, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(m_inner, &rect);
}

void sdl::Renderer::put_texture(Texture& tex, SDL_Rect target)
{
    if (0 != SDL_RenderCopy(m_inner, tex.peek(), nullptr, &target)) {
        throw std::runtime_error("SDL_RenderCopy() failed: " + sdl::get_error());
    }
}

void sdl::Renderer::copy_texture(Texture& tex, SDL_Rect target, SDL_Rect source)
{
    if (0 != SDL_RenderCopy(m_inner, tex.peek(), &source, &target)) {
        throw std::runtime_error("SDL_RenderCopy() failed: " + sdl::get_error());
    }
}

void sdl::Renderer::present()
{
    SDL_RenderPresent(m_inner);
}

// --- Window ---

sdl::Window::Window(std::string title, Size2d size)
{
    uint32_t flags = SDL_WINDOW_ALLOW_HIGHDPI;
    m_inner = SDL_CreateWindow(title.c_str(),
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        size.w, size.h, flags);
    if (!m_inner) {
        throw std::runtime_error("SDL_CreateWindow() failed: " + sdl::get_error());
    }
}

sdl::Window::~Window()
{
    if (m_inner) {
        SDL_DestroyWindow(m_inner);
    }
}
