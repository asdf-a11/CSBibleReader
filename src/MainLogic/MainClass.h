#pragma once
#include <SDL3_ttf/SDL_ttf.h>
#include "Menus/Menu.h"
#include <memory>
#include "Cam.h"

class MainClass{
private:    
    //Store the menu pointer here
    std::unique_ptr<Menu> currentMenuPtr;
    bool changeMenuFlag = false;
    std::unique_ptr<Menu> toSwitchMenu;
    //Store bible class instance here
public:
    TTF_Font* font = nullptr;
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *texture = nullptr;
    Cam cam;
    
public:
    void Init();
    void Update();
    void addEvent(SDL_Event* event);
    void ChangeMenu(std::unique_ptr<Menu> newMenu);
    //TODO destructor that destroys the font as well
private:
    void ForceMenuChange();
};