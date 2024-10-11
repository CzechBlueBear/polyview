#pragma once

#include <string>
#include <vector>
#include <cstdint>

class TextPiece {
public:
    std::string text;
    bool space_follows = false;
    bool tab_follows = false;
    bool empty() const { return text.empty(); }
};

class Line {
public:
    std::vector<TextPiece> pieces;
    bool empty() const { return pieces.empty(); }
};

class Document {
protected:
    std::vector<Line> m_lines;

    void add_line();
    void add_piece(TextPiece const& piece);
public:
    void load(std::string path);
    size_t size() const { return m_lines.size(); }
    Line& get_line(int number);
};
