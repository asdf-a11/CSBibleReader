#include "Menus/MainMenu.h"
#include "MainLogic/MainClass.h"

void MainMenu::Init(MainClass* ptr){

}
void MainMenu::Update(MainClass* ptr){

}
void MainMenu::Draw(MainClass* ptr){
    int screenx;
    int screeny;
    SDL_GetRenderOutputSize(ptr->renderer, &screenx, &screeny);
    SDL_SetRenderDrawColor(ptr->renderer, 0, 0, 0, 255);
    SDL_RenderClear(ptr->renderer);
}