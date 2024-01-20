// doommenu.h

#ifndef DOOMMENU_H
#define DOOMMENU_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include <SDL2/SDL_ttf.h>
TTF_Font *font = NULL;

SDL_Texture *textTexture = NULL;
SDL_Texture *highlightTexture = NULL;

static int screenw = 480;
static int screenh = 320;

// VAR_INC and VAR_DEC courtesy of @kloptops
#define VAR_INC(variable, amount, total) \
    variable = (variable + amount) % total
#define VAR_DEC(variable, amount, total) \
    variable = (variable - amount + total) % total

// min & max clamp for pageup/pagedown courtesy of @kloptops
#define max(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
        _a > _b ? _a : _b; })
#define min(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
        _a < _b ? _a : _b; })

#define MAX_PWADS 500
#define DISPLAY_HEIGHT 14
#define SCROLL_SIZE 8

#define MAX_EPISODES 6
#define MAX_MAPS_PER_EPISODE 9
#define MAX_D2_MAPS 32

#define SKILL_LEVEL 5

#define MAX_BOT_OPTIONS 17

const char *skillLabels[SKILL_LEVEL] = {
    "I'm too young to die",
    "Hey, not too rough",
    "Hurt me plenty",
    "Ultra-Violence",
    "Nightmare"
};

const char *noLevelWarpLabel = "No Level Warp";
const char *mapLabels[MAX_EPISODES * MAX_MAPS_PER_EPISODE + MAX_D2_MAPS + 1] = {
    "No Level Warp"
};

const char* botOptions[MAX_BOT_OPTIONS] = {
    "No Bots",
    "Cooperative, 1 Bot",
    "Cooperative, 2 Bots",
    "Cooperative, 3 Bots",
    "Cooperative, 4 Bots",
    "Cooperative, 5 Bots",
    "Cooperative, 6 Bots",
    "Cooperative, 7 Bots",
    "Cooperative, 8 Bots",
    "Deathmatch, 1 Bot",
    "Deathmatch, 2 Bots",
    "Deathmatch, 3 Bots",
    "Deathmatch, 4 Bots",
    "Deathmatch, 5 Bots",
    "Deathmatch, 6 Bots",
    "Deathmatch, 7 Bots",
    "Deathmatch, 8 Bots"
};

char **files = NULL;

int numSourcePorts = 0;
const char *sourcePorts[0];

const char *gzDoomExecutable = "gzdoom";
const char *lzDoomExecutable = "lzdoom";

int numValidIWADs = 0;
const char **validIWADs = NULL;

/* 
Use of strcasecmp and strcasestr enabled requiring only one entry per name
instead of several to accomodate case sensitivity - courtesy of @kloptops
*/

const char *iwads[] = {
    "doom.wad", "doom2.wad", "plutonia.wad", "tnt.wad", "chex quest 3.wad",
    "chex.wad", "chex3.wad", "heretic.wad", "hexen.wad", "strife1.wad",
    "hacx.wad", "freedoom.pk3", "freedm.wad", "freedoom1.wad", "freedoom2.wad",
    "freedoom.wad", "doom1.wad", "doom2f.wad", "heretic1.wad", "strife0.wad",
    "hacxsw.wad"
};

int numPWADs = 0;
const char **pwadOptions = NULL;

const char *pwadExtensions[] = {
    ".wad", ".pk3", ".zip", ".pwad"
};

#endif // DOOMMENU_H
