#!/usr/bin/env python3

import json
import re
import sys
from pathlib import Path
import copy
import unicodedata

# Splits words and punctuation into separate tokens.
# Example:
# "hello." -> ["hello", "."]
# "word,"  -> ["word", ","]
TOKEN_RE = re.compile(r"\w+|[^\w\s]", re.UNICODE)


def tokenize(text):
    #return TOKEN_RE.findall(text)
    text = copy.deepcopy(text)
    #if "[" in text:
    #    print("Here")
    #text = " ".join(text.replace("]","[").split("[")[0:2:])
    l = list(",.?!()\"[]")
    for i in l:
        text = text.replace(i, " "+i+" ")
    for i in range(1):
        text = text.replace("  ", " ")
    return text.split()


def escape_c_string(s):
    return (
        s.replace("\\", "\\\\")
         .replace('"', '\\"')
         .replace("\n", "\\n")
         .replace("\r", "\\r")
         .replace("\t", "\\t")
    )


def main(input_json, output_header, namespace,
         writeBookNames=True, bibleContents=True):
    with open(input_json, "r", encoding="utf-8") as f:
        bible = json.load(f)

    word_to_index = {}
    unique_words = []

    all_word_indices = []

    books = []
    chapters = []
    verses = []

    current_word_index = 0

    for bookIdx, book in enumerate(bible["books"]):

        book_start = current_word_index
        chapter_start_index = len(chapters)

        for chapterIdx, chapter in enumerate(book["chapters"]):
            print(f"Book {book["name"]} ch {chapterIdx}")

            chapter_start = current_word_index
            verse_start_index = len(verses)

            for verse in chapter["verses"]:

                verse_start = current_word_index

                tokens = tokenize(verse["text"])

                for token in tokens:

                    if token not in word_to_index:
                        word_to_index[token] = len(unique_words)
                        s = unicodedata.normalize('NFC', token)
                        #print("Adding unique word: ",s)
                        unique_words.append(s)

                    idx = word_to_index[token]
                    all_word_indices.append(idx)
                    current_word_index += 1

                verse_end = current_word_index

                verses.append({
                    "start": verse_start,
                    "end": verse_end
                })

            chapter_end = current_word_index

            chapters.append({
                "start": chapter_start,
                "end": chapter_end,
                "firstVerse": verse_start_index,
                "verseCount": len(verses) - verse_start_index
            })

        book_end = current_word_index

        books.append({
            "start": book_start,
            "end": book_end,
            "firstChapter": chapter_start_index,
            "chapterCount": len(chapters) - chapter_start_index
        })

    if len(unique_words) > 2**(4*8)-1:
        raise RuntimeError(
            f"Too many unique words ({len(unique_words)}). "
            "unsigned short can only store 65535 entries."
        )

    #TODO remember to remove
    #return unique_words
    with open(output_header, "w", encoding="utf-8") as out:

        out.write("#pragma once\n\n")
        out.write("#include \"../bibleStructs.h\"\n\n")

        out.write("namespace "+namespace+" {\n")

        if writeBookNames:
            out.write("static const char* book_names[] = {\n")

            for book in bible["books"]:
                out.write("\t\""+book["name"] + "\",\n")


            out.write("};\n\n")
        #
        if bibleContents:
            #
            # Unique words
            #
            out.write(f"static const unsigned int g_wordCount = {len(unique_words)};\n\n")

            out.write("static const char* g_words[] = {\n")

            for word in unique_words:
                out.write(f'    "{escape_c_string(word)}",\n')

            out.write("};\n\n")

            #
            # Entire text as word indices
            #
            out.write(
                f"static const unsigned int g_textWordCount = {len(all_word_indices)};\n\n"
            )

            out.write("static const unsigned int g_text[] = {\n")

            for i, idx in enumerate(all_word_indices):

                if i % 16 == 0:
                    out.write("    ")

                out.write(f"{idx},")

                if i % 16 == 15:
                    out.write("\n")
                else:
                    out.write(" ")

            if len(all_word_indices) % 16:
                out.write("\n")

            out.write("};\n\n")

            #
            # Verses
            #
            out.write(
                f"static const unsigned int g_verseCount = {len(verses)};\n\n"
            )

            out.write("static const VerseRange g_verses[] = {\n")

            for v in verses:
                out.write(
                    f"    {{ {v['start']}, {v['end']} }},\n"
                )

            out.write("};\n\n")

            #
            # Chapters
            #
            out.write(
                f"static const unsigned int g_chapterCount = {len(chapters)};\n\n"
            )

            out.write("static const ChapterRange g_chapters[] = {\n")

            for c in chapters:
                out.write(
                    f"    {{ {c['start']}, {c['end']}, "
                    f"{c['firstVerse']}, {c['verseCount']} }},\n"
                )

            out.write("};\n\n")

            #
            # Books
            #
            out.write(
                f"static const unsigned int g_bookCount = {len(books)};\n\n"
            )

            out.write("static const BookRange g_books[] = {\n")

            for b in books:
                out.write(
                    f"    {{ {b['start']}, {b['end']}, "
                    f"{b['firstChapter']}, {b['chapterCount']} }},\n"
                )

            out.write("};\n")
        #




        out.write("}\n")

    print(f"Wrote {output_header}")
    print(f"Unique words : {len(unique_words)}")
    print(f"Word tokens  : {len(all_word_indices)}")
    print(f"Books        : {len(books)}")
    print(f"Chapters     : {len(chapters)}")
    print(f"Verses       : {len(verses)}")

    return unique_words

cs_to_kjv = {
    "Кни́га пе́рваѧ мѡѷсе́ова бытїѐ": "Genesis",
    "Кни́га втора́ѧ мѡѷсе́ова и҆схо́дъ": "Exodus",
    "Кни́га тре́тїѧ мѡѷсе́ова леѵі́тъ": "Leviticus",
    "Кни́га четве́ртаѧ мѡѷсе́ова чи́сла": "Numbers",
    "Кни́га пѧ́таѧ мѡѷсе́ова второзако́нїе": "Deuteronomy",
    "Кни́га і҆исꙋ́са наѵи́на": "Joshua",
    "Кни́га сꙋді́й і҆и҃левыхъ": "Judges",
    "Кни́га рꙋ́ѳь": "Ruth",
    "Кни́га пе́рваѧ ца́рствъ": "I Samuel",
    "Кни́га втора́ѧ ца́рствъ": "II Samuel",
    "Кни́га тре́тїѧ ца́рствъ": "I Kings",
    "Кни́га четве́ртаѧ ца́рствъ": "II Kings",
    "Кни́га пе́рваѧ паралїпоме́нѡнъ": "I Chronicles",
    "Кни́га втора́ѧ паралїпоме́нѡнъ": "II Chronicles",
    "Кни́га пе́рваѧ є҆́здры": "Ezra",
    "Кни́га неемі́и": "Nehemiah",
    "Кни́га втора́ѧ є҆́здры": None,  # 1 Esdras
    "Кни́га тѡві́та": None,  # Tobit
    "Кни́га і҆ꙋді́ѳъ": None,  # Judith
    "Кни́га є҆сѳи́рь": "Esther",
    "Кни́га і҆́ѡва": "Job",
    "Ѱалти́рь": "Psalms",
    "Кни́га при́тчей соломѡ́нихъ": "Proverbs",
    "Кни́га є҆кклесїа́ста си́рѣчь проповѣ́дника": "Ecclesiastes",
    "Кни́га пѣ́снь пѣ́сней царѧ̀ соломѡ́на": "Song of Solomon",
    "Кни́га премⷣрости соломѡ́ни": None,  # Wisdom of Solomon
    "Кни́га премⷣрости і҆исꙋ́са сы́на сїра́хова": None,  # Sirach
    "Кни́га прⷪ҇ро́ка и҆са́їи": "Isaiah",
    "Кни́га прⷪ҇ро́ка і҆еремі́и": "Jeremiah",
    "Кни́га пла́чь і҆еремі́евъ": "Lamentations",
    "Кни́га посла́нїе і҆еремі́ино": None,  # Letter of Jeremiah
    "Кни́га прⷪ҇ро́ка варꙋ́ха": None,  # Baruch
    "Кни́га прⷪ҇ро́ка і҆езекі́илѧ": "Ezekiel",
    "Кни́га прⷪ҇ро́ка данїи́ла": "Daniel",
    "Кни́га прⷪ҇ро́ка ѡ҆сі́и": "Hosea",
    "Кни́га прⷪ҇ро́ка і҆ѡи́лѧ": "Joel",
    "Кни́га прⷪ҇ро́ка а҆мѡ́са": "Amos",
    "Кни́га прⷪ҇ро́ка а҆вді́а": "Obadiah",
    "Кни́га прⷪ҇ро́ка і҆ѡ́ны": "Jonah",
    "Кни́га прⷪ҇ро́ка мїхе́а": "Micah",
    "Кни́га прⷪ҇ро́ка наꙋ́ма": "Nahum",
    "Кни́га прⷪ҇ро́ка а҆ввакꙋ́ма": "Habakkuk",
    "Кни́га прⷪ҇ро́ка софо́нїи": "Zephaniah",
    "Кни́га прⷪ҇ро́ка а҆гге́а": "Haggai",
    "Кни́га прⷪ҇ро́ка заха́рїи": "Zechariah",
    "Кни́га прⷪ҇ро́ка малахі́и": "Malachi",
    "Кни́га пе́рваѧ маккаве́йскаѧ": None,
    "Кни́га втора́ѧ маккаве́йскаѧ": None,
    "Кни́га тре́тїѧ маккаве́йскаѧ": None,
    "Кни́га тре́тїѧ є҆́здры": None,  # 2 Esdras
    "Ѿ матѳе́а ст҃о́е бл҃говѣствова́нїе": "Matthew",
    "Ѿ ма́рка ст҃о́е бл҃говѣствова́нїе": "Mark",
    "Ѿ лꙋкѝ ст҃о́е бл҃говѣствова́нїе": "Luke",
    "Ѿ і҆ѡа́нна ст҃о́е бл҃говѣствова́нїе": "John",
    "Дѣѧ̑нїѧ ст҃ы́хъ а҆пⷭ҇лъ": "Acts",
    "Собо́рное посла́нїе і҆а́кѡвле": "James",
    "Собо́рное посла́нїе пе́рвое ст҃а́гѡ а҆пⷭ҇ла петра̀": "I Peter",
    "Собо́рное посла́нїе второ́е ст҃а́гѡ а҆пⷭ҇ла петра̀": "II Peter",
    "Собо́рное посла́нїе пе́рвое ст҃а́гѡ а҆пⷭ҇ла і҆ѡа́нна бг҃осло́ва": "I John",
    "Собо́рное посла́нїе второ́е ст҃а́гѡ а҆пⷭ҇ла і҆ѡа́нна бг҃осло́ва": "II John",
    "Собо́рное посла́нїе тре́тїе ст҃а́гѡ а҆пⷭ҇ла і҆ѡа́нна бг҃осло́ва": "III John",
    "Собо́рное посла́нїе і҆ꙋ́дино": "Jude",
    "Посла́нїе къ ри́млѧнѡмъ ст҃а́гѡ а҆пⷭ҇ла па́ѵла": "Romans",
    "Посла́нїе пе́рвое къ корі́нѳѧнѡмъ ст҃а́гѡ а҆пⷭ҇ла па́ѵла": "I Corinthians",
    "Посла́нїе второ́е къ корі́нѳѧнѡмъ ст҃а́гѡ а҆пⷭ҇ла па́ѵла": "II Corinthians",
    "Посла́нїе къ гала́тѡмъ ст҃а́гѡ а҆пⷭ҇ла па́ѵла": "Galatians",
    "Посла́нїе ко є҆фесе́ємъ ст҃а́гѡ а҆пⷭ҇ла па́ѵла": "Ephesians",
    "Посла́нїе къ фїлїпписі́ємъ ст҃а́гѡ а҆пⷭ҇ла па́ѵла": "Philippians",
    "Посла́нїе къ колосса́ємъ ст҃а́гѡ а҆пⷭ҇ла па́ѵла": "Colossians",
    "Посла́нїе пе́рвое къ солꙋ́нѧнѡмъ ст҃а́гѡ а҆пⷭ҇ла па́ѵла": "I Thessalonians",
    "Посла́нїе второ́е къ солꙋ́нѧнѡмъ ст҃а́гѡ а҆пⷭ҇ла па́ѵла": "II Thessalonians",
    "Посла́нїе пе́рвое къ тїмоѳе́ю ст҃а́гѡ а҆пⷭ҇ла па́ѵла": "I Timothy",
    "Посла́нїе второ́е къ тїмоѳе́ю ст҃а́гѡ а҆пⷭ҇ла па́ѵла": "II Timothy",
    "Посла́нїе къ ті́тꙋ ст҃а́гѡ а҆пⷭ҇ла па́ѵла": "Titus",
    "Посла́нїе къ фїлимо́нꙋ ст҃а́гѡ а҆пⷭ҇ла па́ѵла": "Philemon",
    "Посла́нїе ко є҆вре́ємъ ст҃а́гѡ а҆пⷭ҇ла па́ѵла": "Hebrews",
    "А҆пока́лѷѱїсъ ст҃а́гѡ і҆ѡа́нна бг҃осло́ва": "Revelation of John"
}

def normalize_cs(text):
    return unicodedata.normalize("NFD", text)
def strip_diacritics(text):
    text = normalize_cs(text)
    return "".join(
        ch for ch in text
        if unicodedata.category(ch) != "Mn"
    )

def writeTranslationTable(toTranslateUnique, translateToUnique, toTranslateBibelPath, translateToBiblePath):
    #[wordName, [bookName, chapter, verse]]

    cleanMapping = {}
    cleanTranslate = {}
    for i in toTranslateUnique:
        #l = list(".,?!<>\"[]()")
        #if i[0] in l:
        #    continue
        s = i.lower()
        s = strip_diacritics(s)
        cleanTranslate[s] = 0
        cleanMapping[i] = s
    toTranslateUnique = list(cleanTranslate.keys())


    wordOccurence = {}
    with open(toTranslateBibelPath, "r", encoding="utf-8") as f:
        cs_bible = json.load(f)
    for bookIdx, book in enumerate(cs_bible["books"]):
        for chapterIdx, chapter in enumerate(book["chapters"]):            
            for verseIdx,verse in enumerate(chapter["verses"]):
                print(f"{book["name"]} ch {chapterIdx} vr {verseIdx}")
                tokens = tokenize(verse["text"])
                for t in tokens:
                    t = t.lower()
                    t = strip_diacritics(t)
                    for idx,toTranslateWord in enumerate(toTranslateUnique):
                        #if idx % 1 == 0:
                        #    print(idx/len(toTranslateUnique), " Percent left")
                        if t == toTranslateWord:
                            if wordOccurence.get(toTranslateWord) == None:
                                wordOccurence[toTranslateWord] = []
                            wordOccurence[toTranslateWord].append([book["name"], chapterIdx, verseIdx])
            #break#TODO remove
        #break

    print("Gatherd word occurences")


    with open(translateToBiblePath, "r", encoding="utf-8") as f:
        kjv_bible = json.load(f)
    table = {}

    for idx,toTranslate in enumerate(zip(wordOccurence.keys(), wordOccurence.values())):
        print(idx / len(wordOccurence), " percent left")
        table[toTranslate[0]] = {}
        for position in toTranslate[1]:
            bookName = cs_to_kjv[position[0]]
            chapterNumber = position[1]
            verseNumber = position[2]
            if bookName == None:
                continue
            for book in kjv_bible["books"]:
                if book["name"] != bookName:
                    continue                
                try:
                    for token in tokenize(book["chapters"][chapterNumber]["verses"][verseNumber]["text"]):
                        token = token.lower()
                        if table[toTranslate[0]].get(token) == None:
                            table[toTranslate[0]][token] = 0
                        table[toTranslate[0]][token] += 1
                except Exception:
                    print(f"FAILED ON ch {chapterIdx} vr {verseNumber}")

                break
    #print(table)
    return table, cleanMapping
    

        


if __name__ == "__main__":
    kjvUnique = main("packagedData/data/bible.json", "packagedData/data2CFiles/KJV_data.h", "KJV",
         writeBookNames=True, bibleContents=False)
    csUnique = main("packagedData/data/CSlElizabeth-CS.json", "packagedData/data2CFiles/CSlElizabeth-CS_data.h", "Elizabeth")


    translationTable, mappingTable = writeTranslationTable(csUnique, kjvUnique, "packagedData/data/CSlElizabeth-CS.json", "packagedData/data/KJV.json")

    with open("packagedData/data/translationTable.h", "w", encoding="utf-8") as out:
        out.write("#pragma once\n")
        out.write("#include <vector>\n")
        out.write("#include <utility>\n\n")
        out.write("namespace Translations{\n")

        out.write("static const std::pair<unsigned int, unsigned int> wordIndexToCleanIndex[] = {\n")
        for i,j in zip(mappingTable.keys(), mappingTable.values()):
            csIndex = csUnique.index(i)
            cleanIndex = list(translationTable.keys()).index(j)
            out.write("\tstd::pair<unsigned int, unsigned int>(" + str(csIndex) + ", " + str(cleanIndex) + "),\n")
        out.write("};\n\n\n")

        out.write("static const std::pair<unsigned int, std::vector<std::pair<const char*, unsigned int>>> translationTable [] = {\n")

        for idx,i in enumerate(zip(translationTable.keys(), translationTable.values())):
            cleanWord = i[0]
            dictOfTranslations = i[1]
            typ = "std::pair<unsigned int, std::vector<std::pair<const char*, unsigned int>>>"
            inner = "{"
            for j in dictOfTranslations.keys():
                inner += f"std::pair<const char*, unsigned int>(\"{j}\",{dictOfTranslations[j]}),"
            inner += "}"
            out.write(f"\t{typ}({idx}, {inner}),\n")

        out.write("};\n\n")

        out.write("}\n")