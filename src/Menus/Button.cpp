#include "Menus/Button.h"
#include <iostream>

void Button::render(MainClass* ptr){
    SDL_Surface* s = TTF_RenderText_Blended(ptr->font, text.c_str(), 0, {255,255,255});
    SDL_Texture* texture = SDL_CreateTextureFromSurface(ptr->renderer, s);
    SDL_DestroySurface(s);
    float w; float h;
    SDL_GetTextureSize(texture, &w, &h);
    sx = w;
    sy = h;
    SDL_FRect dst = { x,y,w,h };
    SDL_FRect dst2 = { x,y,w,h };
    SDL_SetRenderDrawColor(ptr->renderer, bgColour.r, bgColour.g, bgColour.b, 255);
    SDL_RenderFillRect(ptr->renderer, &dst2);
    SDL_RenderTexture(ptr->renderer, texture, NULL, &dst);  
    SDL_DestroyTexture(texture);
}

bool Button::isClicked(MainClass* ptr){
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