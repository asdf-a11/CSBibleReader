#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Utf8Strings/Utf8String.h"
#include <vector>
#include "preproc.h"
#include <utility>

std::pair<float,float> renderText(SDL_Renderer* renderer,TTF_Font* font, const Utf8String& text, float x, float y){
    //Split based on new lines
    std::vector<Utf8String> lineList = {""};
    for (auto it = text.begin(); it != text.end(); ++it) {
        //std::cout << "[" << *it << "] ";
        if(*it == "\n"){
            lineList.push_back("");
        }
        else{
            lineList.back() += *it;
        }        
    }

    float totalWidth = 0.f;
    float totalHeight = 0.f;
    constexpr float lineGap = 2.f;

    loop(i,0,lineList.size()){
        SDL_Surface* s = TTF_RenderText_Blended(font, lineList[i].c_str(),
            0, {255,255,255});
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, s);
        SDL_DestroySurface(s);
        float w; float h;
        SDL_GetTextureSize(texture, &w, &h);
        totalWidth = std::max(w,totalWidth);
        totalHeight += h + lineGap;
        SDL_FRect dst = { x,y,w,h };
        y += h;
        SDL_RenderTexture(renderer, texture, NULL, &dst);  
        SDL_DestroyTexture(texture);
    }
    return std::pair<float,float>(totalWidth, totalHeight);
}