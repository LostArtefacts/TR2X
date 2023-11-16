#include "game/random.h"

#include "global/funcs.h"
#include "global/vars.h"

#include <time.h>

void __cdecl Random_SeedControl(const int32_t seed)
{
    g_RandControl = seed;
}

void __cdecl Random_SeedDraw(const int32_t seed)
{
    g_RandDraw = seed;
}

void __cdecl Random_Seed(void)
{
    time_t lt = time(0);
    struct tm *tptr = localtime(&lt);
    Random_SeedControl(tptr->tm_sec + 57 * tptr->tm_min + 3543 * tptr->tm_hour);
    Random_SeedDraw(tptr->tm_sec + 43 * tptr->tm_min + 3477 * tptr->tm_hour);
}
