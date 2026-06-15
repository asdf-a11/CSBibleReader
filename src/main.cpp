#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "MainLogic/MainClass.h"
#include <chrono>
#include <string>

//#include <packagedData/packagedData.h>



static MainClass mainClass;

//const unsigned char* tiny_ttf;
//const unsigned int tiny_ttf_len;
//const unsigned char* tiny_ttf = PackagedData::getCSFontDataPtr();
//unsigned int tiny_ttf_len = (unsigned int)PackagedData::getCSFontDataSize();


/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{




    mainClass.Init();

    /* Create the text */
    //text = TTF_RenderText_Blended(font, "HeWorld!", 0, color);
    //if (text) {
    //    texture = SDL_CreateTextureFromSurface(renderer, text);
    //    SDL_DestroySurface(text);
    //}
    //if (!texture) {
    //    SDL_Log("Couldn't create text: %s\n", SDL_GetError());
    //    return SDL_APP_FAILURE;
    //}

    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_KEY_DOWN ||
        event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }

    //mainClass.addEvent(event);

    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    auto start = std::chrono::steady_clock::now();

    mainClass.Update();

    auto end = std::chrono::steady_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    double FPS = 1.0 / ((double)duration.count() * 1.0e-6);    

    SDL_SetWindowTitle(
        mainClass.window, std::to_string(FPS).c_str()
    );

    
/*

    int w = 0, h = 0;
    SDL_FRect dst;
    const float scale = 4.0f;


    SDL_GetRenderOutputSize(renderer, &w, &h);
    SDL_SetRenderScale(renderer, scale, scale);
    SDL_GetTextureSize(texture, &dst.w, &dst.h);
    dst.x = ((w / scale) - dst.w) / 2;
    dst.y = ((h / scale) - dst.h) / 2;


    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, NULL, &dst);
    SDL_RenderPresent(renderer);
*/

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    //if (font) {
    //    TTF_CloseFont(font);
    //}
    TTF_Quit();
}


#include "MainLogic/MainClass.cpp"
#include "Menus/BookMenu.cpp"
#include "Menus/Menu.cpp"
#include "MainLogic/Cam.cpp"
#include "Menus/Button.cpp"
#include "Menus/ChapterMenu.cpp"
#include "Menus/ReadMenu.cpp"
#include "Reading/Word.cpp"