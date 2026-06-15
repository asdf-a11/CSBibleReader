#include "Menus/BookMenu.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "preproc.h"
#include <utility>
#include "../packagedData/packagedData.h"
#include "Utf8Strings/Utf8String.h"
#include "Menus/ChapterMenu.h"

void BookMenu::Init(MainClass* ptr){
    bookButtonList.clear();
    auto [bookNameList, bookNameListSize] = PackagedData::getElizabethBookNames();
    loop(i,0,bookNameListSize){
       bookButtonList.push_back(
        Button(
            50.f, 20.f + (float)i * 30.f, 
            Utf8String(bookNameList[i])
        )
       );
    }   
}
void BookMenu::Update(MainClass* ptr){
    loop(i,0,bookButtonList.size()){
        if(bookButtonList[i].isClicked(ptr)){
            std::unique_ptr<ChapterMenu> chapterMenuPtr = std::make_unique<ChapterMenu>();
            chapterMenuPtr->bookIndex = i;
            ptr->ChangeMenu(std::move(chapterMenuPtr));
        }
    }
}
void BookMenu::Draw(MainClass* ptr){
    int screenx;
    int screeny;
    SDL_GetRenderOutputSize(ptr->renderer, &screenx, &screeny);
    SDL_SetRenderDrawColor(ptr->renderer, 30, 30, 40, 255);
    SDL_RenderClear(ptr->renderer);

    loop(i,0,bookButtonList.size()){
        bookButtonList[i].render(ptr);
    }
    
}