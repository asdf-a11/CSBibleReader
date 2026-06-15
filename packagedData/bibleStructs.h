#pragma once

struct VerseRange{
    unsigned int start;
    unsigned int end;
};

struct ChapterRange {
    unsigned int start;
    unsigned int end;
    unsigned int firstVerse;
    unsigned int verseCount;
};

struct BookRange{
    unsigned int start;
    unsigned int end;
    unsigned int firstChapter;
    unsigned int chapterCount;
};