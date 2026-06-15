#pragma once

#include "Menus/Menu.h"
#include "MainLogic/MainClass.h"
#include "Menus/Button.h"
#include <vector>

class BookMenu : public Menu{
public:
    std::vector<Button> bookButtonList;
    BookMenu (){
        
    }
    void Init(MainClass* ptr);
    void Update(MainClass* ptr);
    void Draw(MainClass* ptr);
};
