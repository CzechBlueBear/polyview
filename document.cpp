#include "document.hpp"
#include <cstring>
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

void Document::add_piece(TextPiece&& piece)
{
    if (m_lines.empty()) {
        m_lines.push_back(Line());
    }
    m_lines.back().pieces.push_back(std::move(piece));
}

void Document::load(std::string path)
{
    FILE* f = fopen(path.c_str(), "rb");
    if (!f) {
        throw std::runtime_error("could not open file for reading: " + path);
    }

    for(;;) {
        Line line;
        auto c = fgetc(f);

        // line per line
        int column = 0;
        std::stringstream buf;
        for(;;) {
            if (flag_coalesce_spaces && (c == ' ' || c == '\t')) {

                // skip a run of whitespaces
                while (c == ' ' || c == '\t') {
                    c = fgetc(f);
                }
                ungetc(c, f);

                if (!buf.str().empty()) {
                    TextPiece piece(buf.str());
                    add_piece(std::move(piece));
                }
                buf = std::stringstream();
            }
            else if (c == '\n' || c == EOF) {
                if (!buf.str().empty()) {
                    TextPiece piece(buf.str());
                    add_piece(std::move(piece));
                }
                add_line();
                buf = std::stringstream();
                break;
            }
            else if (c < ' ') {     // nonprintable characters
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
        throw std::out_of_range("invalid line number: #" + std::to_string(number));
    }
    if (size_t(number) < m_lines.size()) {
        return m_lines.at(number);
    }
    throw std::out_of_range("line not found: #" + std::to_string(number));
}
