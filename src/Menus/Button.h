#pragma once

#include "../Utf8Strings/Utf8String.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "preproc.h"
#include "MainLogic/MainClass.h"

class Button{
public:
    float x;
    float y;
    //float sx;
    //float sy;
    float sx = 1.f;
    float sy = 1.f;

    SDL_Colour bgColour = {50,50,70};

    Utf8String text;

    Button(){}
    Button(
        float x, float y,
        //float sx, float sy,
        Utf8String text
    ){
        this->x = x;
        this->y = y;
        //this->sx = sx;
        //this->sy = sy;
        this->text = text;
    }

    bool isClicked(MainClass* ptr);
    void render(MainClass* ptr);
};