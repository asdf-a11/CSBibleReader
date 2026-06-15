#include <utility>

#include "packagedData.h"
#include "bibleStructs.h"

#include "data2CFiles/Ponomar-Regular.c"
#include "data2CFiles/CSlElizabeth-CS_data.h"
#include "data2CFiles/KJV_data.h"





namespace PackagedData{
    std::pair<const unsigned char*, const int> getCSFontData(){
        return {CSFontData, CSFontData_length};
    }
    std::pair<const char**, const int> getElizabethBookNames(){
        return std::pair<const char**, const int>(Elizabeth::book_names, Elizabeth::g_bookCount);
    }
    std::pair<const BookRange*, const int> getElizabethBooks(){
        return std::pair<const BookRange*, const int>(Elizabeth::g_books, Elizabeth::g_bookCount);
    }
    std::pair<const ChapterRange*, const int> getElizabethChapters(){
        return std::pair<const ChapterRange*, const int>(Elizabeth::g_chapters, Elizabeth::g_chapterCount);
    }
    std::pair<const VerseRange*, const int> getElizabethVerses(){
        return std::pair<const VerseRange*, const int>(Elizabeth::g_verses, Elizabeth::g_verseCount);
    }
    std::pair<const unsigned int*, const int> getElizabethText(){
        return std::pair<const unsigned int*, const int>(Elizabeth::g_text, Elizabeth::g_textWordCount);
    }
    std::pair<const char**, const int> getElizabethUniqueWords(){
        return std::pair<const char**, const int>(Elizabeth::g_words, Elizabeth::g_wordCount);
    }
}