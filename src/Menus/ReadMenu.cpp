#include "Menus/ReadMenu.h"
#include <memory>
#include "../packagedData/bibleStructs.h"
#include "../packagedData/packagedData.h"
#include "renderText.h"

void ReadMenu::Init(MainClass* ptr){
    wordList.clear();
    
    auto [bookList, bookNumber] = PackagedData::getElizabethBooks();
    int chapterIndexInGlobalChapterTable = bookList[bookIndex].firstChapter + chapterIndex;

    auto [chapterList, chapterNumber] = PackagedData::getElizabethChapters();
    int firstVerseIndex = chapterList[chapterIndexInGlobalChapterTable].firstVerse;
    int verseCount = chapterList[chapterIndexInGlobalChapterTable].verseCount;

    loop(verseCounter,0,verseCount){
        auto [verseList, totalNumberOfVerses] = PackagedData::getElizabethVerses();
        int verseStart = verseList[verseCounter].start;
        int verseEnd = verseList[verseCounter].end;
        int verseLength = verseEnd - verseStart;
        auto [wordList, totalWordNumber] = PackagedData::getElizabethText();
        loop(wordCounter, 0, verseLength){
            this->wordList.push_back(
                Word(wordList[wordCounter + verseStart])
            );
        }
    }
}
void ReadMenu::Update(MainClass* ptr){
    loop(i,0,wordList.size()){
        if(wordList[i].isClicked(ptr)){
            //TODO update word status
            //showTranslationWindow = true;
            setTranslationText(ptr, wordList[i].wordIndex);
        }
    }
}
void ReadMenu::Draw(MainClass* ptr){
    int screenx;
    int screeny;
    SDL_GetRenderOutputSize(ptr->renderer, &screenx, &screeny);
    SDL_SetRenderDrawColor(ptr->renderer, 30, 30, 40, 255);
    SDL_RenderClear(ptr->renderer);


    float xPadding = 10.f;
    float yPadding = 10.f;
    float lineGap = 35.f;
    float wordGap = 10.f;
    float xPos = xPadding;
    float yPos = yPadding;
    loop(i,0,wordList.size()){
        wordList[i].render(ptr, xPos, yPos);
        xPos += wordList[i].sx + wordGap;
        if(xPos >= (float)screenx - xPadding - wordList[i].sx){
            yPos += lineGap;
            xPos = xPadding;
        }
    }

    //Drawing the translation window
    //if(showTranslationWindow){
    renderTranslationMenu(ptr);
    //}
}

void ReadMenu::setTranslationText(MainClass* ptr, unsigned int wordIndex){

    translationText = "Testing testing\n testing 2";
}

void ReadMenu::renderTranslationMenu(MainClass* ptr){
    int screenx;
    int screeny;
    SDL_GetRenderOutputSize(ptr->renderer, &screenx, &screeny);
    SDL_FRect translationWindowRect = {
        0,
        (float)screeny * (2.f/3.f),
        (float)screenx,
        (float)screeny * (1.f/3.f)
    };
    SDL_SetRenderDrawColor(ptr->renderer, 100, 100, 120, 255);
    SDL_RenderFillRect(ptr->renderer, &translationWindowRect);

    
    renderText(ptr->renderer, ptr->font, translationText, 
        translationWindowRect.x + 2.f, translationWindowRect.y + 2.f);
}