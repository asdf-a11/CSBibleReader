#include "Menus/ChapterMenu.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "MainLogic/MainClass.h"
#include "preproc.h"
#include "../packagedData/bibleStructs.h"
#include "../packagedData/packagedData.h"
#include "Menus/ReadMenu.h"

void ChapterMenu::Init(MainClass* ptr){
    chapterButtonList.clear();
    
    auto [bookList, bookNumber] = PackagedData::getElizabethBooks();
    //TODO some error checking might be good
    int chapterNumber = bookList[bookIndex].chapterCount;


    loop(i,0,chapterNumber){
        chapterButtonList.push_back(
            Button(
                50.f, 20.f + (float)i * 30.f, 
                Utf8String(std::to_string(i))
            )
        );
    }
}
void ChapterMenu::Update(MainClass* ptr){
    loop(i,0,chapterButtonList.size()){
        if(chapterButtonList[i].isClicked(ptr)){
            std::unique_ptr<ReadMenu> readMenu = std::make_unique<ReadMenu>();
            readMenu->bookIndex = bookIndex;
            readMenu->chapterIndex = i;
            ptr->ChangeMenu(std::move(readMenu));
        }
    }
}
void ChapterMenu::Draw(MainClass* ptr){
    int screenx;
    int screeny;
    SDL_GetRenderOutputSize(ptr->renderer, &screenx, &screeny);
    SDL_SetRenderDrawColor(ptr->renderer, 30, 30, 40, 255);
    SDL_RenderClear(ptr->renderer);

    loop(i,0,chapterButtonList.size()){
        chapterButtonList[i].render(ptr);
    }

}