#pragma once
#include "Menus/Menu.h"
#include "MainLogic/MainClass.h"

class MainMenu : public Menu{
public:
    MainMenu(){
        
    }
    void Init(MainClass* ptr);
    void Update(MainClass* ptr);
    void Draw(MainClass* ptr);
};
