#pragma once

#include "Menus/Menu.h"
#include <vector>
#include "Menus/Button.h"
#include "Reading/Word.h"

class ReadMenu final : public Menu{
public:
    
    int bookIndex;
    int chapterIndex;

    std::vector<Word> wordList;

    //bool showTranslationWindow = false;
    Utf8String translationText = "";

    ReadMenu(){
        
    }
    void Init(MainClass* ptr);
    void Update(MainClass* ptr);
    void Draw(MainClass* ptr);
private:
    void renderTranslationMenu(MainClass* ptr);
    void setTranslationText(MainClass* ptr, unsigned int wordIndex);
};