#pragma once

#include <string>
#include <vector>
#include "sdl_wrapper.hpp"

class TextPiece {
protected:
    std::string m_text;
public:
    TextPiece();
    explicit TextPiece(std::string const& text) : m_text(text) {}
    TextPiece(TextPiece& other) = delete;
    TextPiece(TextPiece&& other) = default;
    bool empty() const { return m_text.empty(); }
    std::string get_text() const { return m_text; }
};

class Line {
public:
    std::vector<TextPiece> pieces;
    Line() {}
    Line(Line& other) = delete;
    Line(Line&& other) = default;
    bool empty() const { return pieces.empty(); }
};

class Document {
protected:
    std::vector<Line> m_lines;
    std::shared_ptr<sdl::Font> m_font;
    sdl::Color m_text_color = sdl::Color::BLACK;

    void add_line();
    void add_piece(TextPiece&& piece);
public:
    Document(std::shared_ptr<sdl::Font> font) : m_font(font) {}
    void load(std::string path);
    size_t size() const { return m_lines.size(); }
    Line& get_line(int number);
};
