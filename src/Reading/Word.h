#pragma once

#include "MainLogic/MainClass.h"
#include "Utf8Strings/Utf8String.h"

class Word{
public:
    float x;
    float y;
    float sx;
    float sy;

    unsigned int wordIndex;

    SDL_Colour bgColour = {50,50,70};

    Word(){}
    Word(unsigned int wordIndex){
        this->wordIndex = wordIndex;
    }

    const char* asConstCharPtr();
    Utf8String asString();

    bool isClicked(MainClass* ptr);
    void render(MainClass* ptr, float x, float y);
};