#include "Reading/Word.h"
#include "MainLogic/MainClass.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Utf8Strings/Utf8String.h"
#include "../packagedData/packagedData.h"

const char* Word::asConstCharPtr(){
    auto [uniqueWordList, uniqueWordCount] = PackagedData::getElizabethUniqueWords();
    const char* wordPtr = uniqueWordList[wordIndex];
    return wordPtr;
}

Utf8String Word::asString(){
    return Utf8String(asConstCharPtr());
}

bool Word::isClicked(MainClass* ptr){
    float mouseX, mouseY;
    SDL_MouseButtonFlags mouseState = SDL_GetMouseState(&mouseX, &mouseY);

    if(mouseX > x && mouseX < x + sx){
        if(mouseY  > y && mouseY < y + sy){            
            if(mouseState & SDL_BUTTON_LMASK){
                return true;
            }
        }
    }
    return false;
}

void Word::render(MainClass* ptr, float x, float y){
    //TODO dont render the word if it is off the screen
    this->x = x;
    this->y = y;
    SDL_Surface* s = TTF_RenderText_Blended(ptr->font, asConstCharPtr(), 0, {255,255,255});
    SDL_Texture* texture = SDL_CreateTextureFromSurface(ptr->renderer, s);
    SDL_DestroySurface(s);
    float w; float h;
    SDL_GetTextureSize(texture, &w, &h);
    sx = w;
    sy = h;
    SDL_FRect dst = { x,y,w,h };
    SDL_RenderTexture(ptr->renderer, texture, NULL, &dst);  
    SDL_DestroyTexture(texture);
}