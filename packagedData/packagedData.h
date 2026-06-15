#pragma once

#include <utility>
#include "bibleStructs.h"


namespace PackagedData{
    std::pair<const unsigned char*, const int> getCSFontData();
    std::pair<const char**, const int> getElizabethBookNames();
    std::pair<const BookRange*, const int> getElizabethBooks();
    std::pair<const ChapterRange*, const int> getElizabethChapters();
    std::pair<const VerseRange*, const int> getElizabethVerses();
    std::pair<const unsigned int*, const int> getElizabethText();
    std::pair<const char**, const int> getElizabethUniqueWords();
}


