#include "MainLogic/MainClass.h"
#include "Menus/Menu.h"
#include "Menus/BookMenu.h"
#include <cassert>
#include "../packagedData/packagedData.h"

void MainClass::Init(){
    SDL_Init(SDL_INIT_VIDEO);

    TTF_Init();

    /* Open the font */
    //font = TTF_OpenFont("Fonts/Ponomar-Regular.ttf", 18.0f);
    auto [fontTTFPtr, fontTTFSize] = PackagedData::getCSFontData();
    //const char* file = SDL_IOFromConstMem(fontTTFPtr, fontTTFSize);
    //font = TTF_OpenFont(file, 18.0f);
    SDL_IOStream* io = SDL_IOFromConstMem(fontTTFPtr, fontTTFSize);
    if(io == nullptr){
        assert(false);
    }
    font = TTF_OpenFontIO(io, true, 25.0f);
    if (!font) {
        SDL_Log("Couldn't open font: %s\n", SDL_GetError());
        assert(false);
    }
    SDL_Color color = { 255, 255, 255, SDL_ALPHA_OPAQUE };
    SDL_Surface *text;
    //SDL_WINDOW_FULLSCREEN
    if (!SDL_CreateWindowAndRenderer("Hello World", 800, 600, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s\n", SDL_GetError());
        assert(false);
    }

    if (!TTF_Init()) {
        SDL_Log("Couldn't initialize SDL_ttf: %s\n", SDL_GetError());
        assert(false);
    }

    currentMenuPtr = std::make_unique<BookMenu>();
    currentMenuPtr.get()->Init(this);
}

void MainClass::Update(){
    //SDL_PumpEvents();

    currentMenuPtr.get()->Update(this);
    currentMenuPtr.get()->Draw(this);

    ForceMenuChange();
    SDL_RenderPresent(renderer);
}

void MainClass::ForceMenuChange(){
    if(changeMenuFlag){
        changeMenuFlag = false;
        currentMenuPtr = std::move(toSwitchMenu);
        currentMenuPtr->Init(this);
    }
}

void MainClass::ChangeMenu(std::unique_ptr<Menu> newMenu){
    changeMenuFlag = true;
    toSwitchMenu = std::move(newMenu);    
}

void MainClass::addEvent(SDL_Event* event){
    
}