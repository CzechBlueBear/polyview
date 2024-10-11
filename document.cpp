#include "document.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include <sys/stat.h>
#include <sys/mman.h>
#include <cassert>
#include <iostream>
#include <sstream>

void Document::add_line()
{
    m_lines.push_back(Line());
}

void Document::add_piece(TextPiece const& piece)
{
    if (m_lines.empty()) {
        m_lines.push_back(Line());
    }
    m_lines.back().pieces.push_back(piece);
}

void Document::load(std::string path)
{
    FILE* f = fopen(path.c_str(), "rb");

    const uint16_t COLUMNS_PER_TAB = 16;

    for(;;) {
        Line line;
        auto c = fgetc(f);

        // line per line
        int column = 0;
        std::stringstream buf;
        for(;;) {
            if (c == ' ' || c == '\t') {

                // skip a run of whitespaces
                while (c == ' ' || c == '\t') {
                    c = fgetc(f);
                }
                ungetc(c, f);

                if (!buf.str().empty()) {
                    TextPiece piece;
                    piece.text = buf.str();
                    if (c == ' ') { piece.space_follows = true; }
                    else if (c == '\t') { piece.tab_follows = true; }
                    add_piece(piece);
                    std::cout << "piece: \"" << piece.text << "\"\n";
                }
                buf = std::stringstream();
            }
            else if (c == '\n' || c == EOF) {
                if (!buf.str().empty()) {
                    TextPiece piece;
                    piece.text = buf.str();
                    add_piece(piece);
                    std::cout << "piece: \"" << piece.text << "\" (+endl)\n";
                }
                add_line();
                buf = std::stringstream();
                break;
            }
            else if (c < ' ') {
                buf.put('?');
            }
            else {
                buf.put(c);
                column++;
            }
            c = fgetc(f);
        }
        if (c == EOF) break;
    }
    fclose(f);
}

Line& Document::get_line(int number)
{
    if (number < 0) {
        throw std::logic_error("line number must not be <0");
    }
    if (size_t(number) < m_lines.size()) {
        return m_lines.at(number);
    }
    throw std::logic_error("no such line");
}
