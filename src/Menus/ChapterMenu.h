#pragma once

#include "Menus/Menu.h"
#include <vector>
#include "Menus/Button.h"

class ChapterMenu final : public Menu{
public:
    int bookIndex;

    std::vector<Button> chapterButtonList;

    ChapterMenu (){
        
    }
    void Init(MainClass* ptr);
    void Update(MainClass* ptr);
    void Draw(MainClass* ptr);
};