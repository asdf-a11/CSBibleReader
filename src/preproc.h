#pragma once

#define loop(i,l,h) for(int i = (l); i < (h); i++)
#define pass (0(void*))

#undef SDL_Colour
#define SDL_Colour SDL_Color