/* Doom Menu for Anbernic and friends by Slayer366 (Andrew A. Kratz) */
#include "doommenu.h"

// Function to handle SDL2 events
int handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            // Handle quit event (e.g., user closes the window)
            return 1; // Signal to exit the loop or program
        } else if (event.type == SDL_KEYDOWN) {
            // Handle keydown events
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    return SDLK_UP;
                case SDLK_DOWN:
                    return SDLK_DOWN;
                case SDLK_PAGEUP:
                    return SDLK_PAGEUP;
                case SDLK_PAGEDOWN:
                    return SDLK_PAGEDOWN;
                case SDLK_RETURN:
                    return SDLK_RETURN;
                case SDLK_ESCAPE:
                    return SDLK_ESCAPE;
            }
        } else if (event.type == SDL_CONTROLLERBUTTONDOWN) {
            // Handle controller events
            switch (event.cbutton.button) {
                case SDL_CONTROLLER_BUTTON_DPAD_UP:
                    return SDLK_UP;
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                    return SDLK_DOWN;
                case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                    return SDLK_PAGEUP;
                case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                    return SDLK_PAGEDOWN;
                case SDL_CONTROLLER_BUTTON_A:
                case SDL_CONTROLLER_BUTTON_B:
                    return SDLK_RETURN;
                case SDL_CONTROLLER_BUTTON_BACK:
                    return SDLK_ESCAPE;
            }
        }
    }
    return 0;
}

// Function to check if a file exists
int fileExists(const char *filename) {
    return access(filename, F_OK) != -1;
}

// Function to search for executables in the specified directory
// strcmp -> strcasecmp courtesy of @kloptops
void searchExecutables(const char *directory, const char *executableName) {
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(directory)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (strcasecmp(ent->d_name, executableName) == 0) {
                // Found the executable
                char *path = malloc(strlen(directory) + strlen(executableName) + 2);
                sprintf(path, "%s/%s", directory, executableName);
                files = realloc(files, (numSourcePorts + 1) * sizeof(char *));
                files[numSourcePorts++] = path;
            }
        }
        closedir(dir);
    }
}

// Function to detect and store paths of GZDoom and LZDoom executables
void detectExecutables() {
    searchExecutables("/usr/bin", gzDoomExecutable);
    searchExecutables("/usr/bin", lzDoomExecutable);
    searchExecutables("/opt/gzdoom", gzDoomExecutable);
    searchExecutables("/opt/lzdoom", lzDoomExecutable);
    searchExecutables("/usr/games", gzDoomExecutable);
    searchExecutables("/usr/games", lzDoomExecutable);
}

// Function to check if a file has a valid IWAD filename
// strcmp -> strcasecmp courtesy of @kloptops
int isValidIWAD(const char *filename) {
    for (int i = 0; i < sizeof(iwads) / sizeof(iwads[0]); i++) {
        if (strcasecmp(filename, iwads[i]) == 0) {
            return 1; // Valid IWAD
        }
    }
    return 0; // Not a valid IWAD
}

// Function to detect if any valid required Doom IWADs are present in the directory and its subdirectories
// strcmp -> strcasecmp courtesy of @kloptops
void detectValidIWADs(const char *directory) {
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(directory)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", directory, ent->d_name);

            // Use stat to check if it's a directory
            struct stat statbuf;
            if (stat(path, &statbuf) != 0) {
                continue;
            }

            if (S_ISDIR(statbuf.st_mode) && strcasecmp(ent->d_name, ".") != 0 && strcasecmp(ent->d_name, "..") != 0) {
                // Recursive call to search in subdirectory
                detectValidIWADs(path);
            } else {
                // Trimming leading "./" from the path
                if (strncmp(path, "./", 2) == 0) {
                    memmove(path, path + 2, strlen(path) - 1);
                }

                // Check if it's a valid IWAD
                if (isValidIWAD(ent->d_name)) {
                    numValidIWADs++;
                    validIWADs = realloc(validIWADs, numValidIWADs * sizeof(char *));
                    validIWADs[numValidIWADs - 1] = strdup(path);
                }
            }
        }
        closedir(dir);
    }
}

// Function to detect if any valid optional PWADs are present in the directory and its subdirectories
// strcmp -> strcasecmp courtesy of @kloptops
void detectValidPWADs(const char *directory) {
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(directory)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", directory, ent->d_name);

            // Use stat to check if it's a directory
            struct stat statbuf;
            if (stat(path, &statbuf) != 0) {
                continue;
            }

            if (S_ISDIR(statbuf.st_mode) && strcasecmp(ent->d_name, ".") != 0 && strcasecmp(ent->d_name, "..") != 0) {
                // Recursive call to search in subdirectory
                detectValidPWADs(path);
            } else {
                // Trimming leading "./" from the path
                if (strncmp(path, "./", 2) == 0) {
                    memmove(path, path + 2, strlen(path) - 1);
                }

                // Check if it's a valid PWAD
                // strstr -> strcasestr courtesy of @kloptops
                for (int i = 0; i < sizeof(pwadExtensions) / sizeof(pwadExtensions[0]); i++) {
                    if (strcasestr(ent->d_name, pwadExtensions[i]) != NULL) {
                        numPWADs++;
                        pwadOptions = realloc(pwadOptions, numPWADs * sizeof(char *));
                        pwadOptions[numPWADs - 1] = strdup(path);
                    }
                }
            }
        }
        closedir(dir);
    }
}

// Function to warp to selected maps
void initializeMapLabels() {
    int index = 1;

    // Generate "eXmY" format for episodes
    for (int episode = 1; episode <= MAX_EPISODES; episode++) {
        for (int map = 1; map <= MAX_MAPS_PER_EPISODE; map++) {
            char label[8];
            snprintf(label, sizeof(label), "e%dm%d", episode, map);
            mapLabels[index++] = strdup(label);
        }
    }

    // Generate "mapXX" format for Doom 2 maps
    for (int map = 1; map <= MAX_D2_MAPS; map++) {
        char label[8];
        snprintf(label, sizeof(label), "MAP%02d", map);
        mapLabels[index++] = strdup(label);
    }
}

// Function to display a menu with existing skill levels
void displaySkillMenu() {
    for (int i = 0; i < SKILL_LEVEL; i++) {
        char buffer[100];
        snprintf(buffer, sizeof(buffer), "%s", skillLabels[i]);
    }
}

// Function to free allocated memory
void freeMemory() {
    for (int i = 0; i < numSourcePorts; i++) {
        free(files[i]);
    }
    free(files);
}

void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y, int r, int g, int b) {
    SDL_Color textColor = {r, g, b, 255};
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, textColor);

    if (textSurface) {
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface);

        SDL_Rect dstRect = {x, y, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &dstRect);
        SDL_DestroyTexture(textTexture);
    }
}

// Function to display a menu with existing files
void displayMenu(SDL_Renderer *renderer, const char **options, int numOptions, int selected, const char *title, int *displayStart) {
    int displayEnd = *displayStart + DISPLAY_HEIGHT;

    // Adjust the display end index to prevent going beyond the array bounds
    if (displayEnd > numOptions) {
        displayEnd = numOptions;
    }

    // Make sure selected doesn't fall below 0
    if (selected < 0) {
        selected = 0;
    }

    // Adjust displayStart based on the selected index
    if (selected < *displayStart) {
        *displayStart = selected;
    } else if (selected >= *displayStart + DISPLAY_HEIGHT) {
        *displayStart = selected - DISPLAY_HEIGHT + 1;
    }

    // Clear the screen
    //SDL_SetRenderDrawColor(renderer, 48, 8, 0, 255); // Dark Red
    SDL_SetRenderDrawColor(renderer, 56, 56, 56, 255); // Gray
    SDL_RenderClear(renderer);

    // Render title
    renderText(renderer, font, title, 10, 10, 224, 210, 112); // Yellow

    // Render options
        for (int i = *displayStart; i < displayEnd; i++) {
            if (i == selected) {
            // Highlight the selected option
            SDL_SetRenderDrawColor(renderer, 0, 0, 128, 255); // Dark Blue
            SDL_Rect rect = {10, 31 + (i - *displayStart) * 20, screenw - 40, 20};
            SDL_RenderFillRect(renderer, &rect);
        }
        //renderText(renderer, font, options[i], 20, 30 + (i - displayStart) * 20, 224, 224, 224); // Dim White
        renderText(renderer, font, options[i], 20, 30 + (i - *displayStart) * 20, 224, 8, 8); // Dark Red
    }
    SDL_RenderPresent(renderer);
}

// Function to compare strings for qsort
// strcmp -> strcasecmp courtesy of @kloptops
int compareStrings(const void *a, const void *b) {
    return strcasecmp(*(const char **)a, *(const char **)b);
}

// Function to run the command
/*
 Originally it would allow for this application to exit immediately after launching child process,
 but this wasn't great because emulationstation would detect that this process had ended and then
 relaunched leaving Doom running in the background while emulationstation was brought into focus.
 The idea was for the application to quit so a shell script would end gptokeyb so it wouldn't
 interfere with the controls in Doom (which it absolutely will).  So we have instead decided to
 kill gptokeyb within this program directly which seems to do the job nicely.
*/
void runCommand(const char *command) {

    //system("pkill -f gptokeyb");
    //system("sudo kill -9 $(pidof gptokeyb)");
    //system("kill -9 $(pidof gptokeyb)");
    system("cp bots.cfg ~/.config/lzdoom/bots.cfg");
    system("cp bots.cfg ~/.config/gzdoom/bots.cfg");

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process
        if (execlp("sh", "sh", "-c", command, NULL) == -1) {
            perror("execlp");
            SDL_Quit();
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process
        //SDL_Quit();
        //exit(0);
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
        }
    }
}

// The main program loop
int main() {

    const char *gp2keyb = getenv("GPTOKEYB");
    if (!gp2keyb) {
        fprintf(stderr, "Warning: GPTOKEYB environment variable not found or not set.\n");
    }

    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        fprintf(stderr, "SDL2 initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL2_ttf for text rendering
    if (TTF_Init() < 0) {
        fprintf(stderr, "SDL2_ttf initialization failed: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a window and renderer
    SDL_Window *window = SDL_CreateWindow("Doom Menu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenw, screenh, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(renderer, screenw, screenh);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    //Load font
    font = TTF_OpenFont("DooM.ttf", 14);
    if (!font) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Open the first available game controller
    SDL_GameController *controller = NULL;
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            controller = SDL_GameControllerOpen(i);
            if (controller) {
                printf("Game controller connected: %s\n", SDL_GameControllerName(controller));
                break;
            }
        }
    }

    if (!controller) {
        printf("Warning: No game controller found.\n");
    }


    detectExecutables();

    if (numSourcePorts == 0) {
        printf("Error: No Doom executables found.\n");
        return 1;
    }

    // Sort the detected executables
    qsort(files, numSourcePorts, sizeof(char *), compareStrings);

    int sourcePortSelected = 0;
    int iwadSelected = 0;
    int firstpwadSelected = -1;
    int secondpwadSelected = -1;
    int thirdpwadSelected = -1;
    int displayStart = 0;

/* Improved event handling, removed redundancies, added boolean operator - courtesy of @kloptops */

bool in_loop = true;

// Source port selection loop
in_loop = true;
while(in_loop) { 
    int key;

    while (1) {
        key = handleEvents();
        if (key == 0) {
            // No more events
            break;
        }

        if (key == SDLK_RETURN) {
            in_loop = false;
            break; // Exit loop on Enter key
        }

        switch (key) {
            case SDLK_UP:
                VAR_DEC(sourcePortSelected, 1, numSourcePorts);
                break;
            case SDLK_DOWN:
                VAR_INC(sourcePortSelected, 1, numSourcePorts);
                break;
            case SDLK_ESCAPE:
                SDL_Quit();
                exit(0);
                break;
        }
    }
    displayMenu(renderer, (const char **)files, numSourcePorts, sourcePortSelected, "Select a Doom Source Port:", &displayStart);
    SDL_Delay(35); // Moved displayMenu function call and added SDL_Delay to reduce unnecessary CPU usage - courtesy of @kloptops
}

    displayStart = 0;
    detectValidIWADs(".");

    // Sort the valid IWADs
    qsort(validIWADs, numValidIWADs, sizeof(char *), compareStrings);

in_loop = true;
while(in_loop) { 
    int key;

    while (1) {
        key = handleEvents();
        if (key == 0) {
            // No more events
            break;
        }

        if (key == SDLK_RETURN) {
            in_loop = false;
            break; // Exit loop on Enter key
        }

        switch (key) {
            case SDLK_UP:
                VAR_DEC(iwadSelected, 1, numValidIWADs);
                break;
            case SDLK_DOWN:
                VAR_INC(iwadSelected, 1, numValidIWADs);
                break;
            case SDLK_PAGEUP:
                    iwadSelected = max(0, iwadSelected - SCROLL_SIZE);
                break;
            case SDLK_PAGEDOWN:
                    iwadSelected = min(numValidIWADs - 1, iwadSelected + SCROLL_SIZE);
                break;
            case SDLK_ESCAPE:
                SDL_Quit();
                exit(0);
                break;
        }
    }
    displayMenu(renderer, validIWADs, numValidIWADs, iwadSelected, "Select an IWAD:", &displayStart);
    SDL_Delay(35); // Moved displayMenu function call and added SDL_Delay to reduce unnecessary CPU usage - courtesy of @kloptops
}

    displayStart = 0;
// Detect valid PWADs & Sort the PWAD options for all PWAD selection screens and display 1st PWAD selection menu
    detectValidPWADs(".");

    qsort(pwadOptions, numPWADs, sizeof(char *), compareStrings);

in_loop = true;
while(in_loop) { 
    int key;

    while (1) {
        key = handleEvents();
        if (key == 0) {
            // No more events
            break;
        }

        if (key == SDLK_RETURN) {
            in_loop = false;
            break; // Exit loop on Enter key
        }

        switch (key) {
            case SDLK_UP:
                    VAR_DEC(firstpwadSelected, 1, numPWADs);
                break;
            case SDLK_DOWN:
                    VAR_INC(firstpwadSelected, 1, numPWADs);
                break;
            case SDLK_PAGEUP:
                    firstpwadSelected = max(0, firstpwadSelected - SCROLL_SIZE);
                break;
            case SDLK_PAGEDOWN:
                    firstpwadSelected = min(numPWADs - 1, firstpwadSelected + SCROLL_SIZE);
                break;
            case SDLK_ESCAPE:
                SDL_Quit();
                exit(0);
                break;
        }
    }
    displayMenu(renderer, pwadOptions, numPWADs, firstpwadSelected, "Select 1st optional PWAD (1 of 3):", &displayStart);
    SDL_Delay(35); // Moved displayMenu function call and added SDL_Delay to reduce unnecessary CPU usage - courtesy of @kloptops
}

    displayStart = 0;
// Display 2nd PWAD selection menu
in_loop = true;
while(in_loop) { 
    int key;

    while (1) {
        key = handleEvents();
        if (key == 0) {
            // No more events
            break;
        }

        if (key == SDLK_RETURN) {
            in_loop = false;
            break; // Exit loop on Enter key
        }

        switch (key) {
            case SDLK_UP:
                    VAR_DEC(secondpwadSelected, 1, numPWADs);
                break;
            case SDLK_DOWN:
                    VAR_INC(secondpwadSelected, 1, numPWADs);
                break;
            case SDLK_PAGEUP:
                    secondpwadSelected = max(0, secondpwadSelected - SCROLL_SIZE);
                break;
            case SDLK_PAGEDOWN:
                    secondpwadSelected = min(numPWADs - 1, secondpwadSelected + SCROLL_SIZE);
                break;
            case SDLK_ESCAPE:
                SDL_Quit();
                exit(0);
                break;
        }
    }
    displayMenu(renderer, pwadOptions, numPWADs, secondpwadSelected, "Select 2nd optional PWAD (2 of 3):", &displayStart);
    SDL_Delay(35); // Moved displayMenu function call and added SDL_Delay to reduce unnecessary CPU usage - courtesy of @kloptops
}

    displayStart = 0;
// Display 3rd PWAD selection menu
in_loop = true;
while(in_loop) { 
    int key;

    while (1) {
        key = handleEvents();
        if (key == 0) {
            // No more events
            break;
        }

        if (key == SDLK_RETURN) {
            in_loop = false;
            break; // Exit loop on Enter key
        }
    
        switch (key) {
            case SDLK_UP:
                    VAR_DEC(thirdpwadSelected, 1, numPWADs);
                break;
            case SDLK_DOWN:
                    VAR_INC(thirdpwadSelected, 1, numPWADs);
                break;
            case SDLK_PAGEUP:
                    thirdpwadSelected = max(0, thirdpwadSelected - SCROLL_SIZE);
                break;
            case SDLK_PAGEDOWN:
                    thirdpwadSelected = min(numPWADs - 1, thirdpwadSelected + SCROLL_SIZE);
                break;
            case SDLK_ESCAPE:
                SDL_Quit();
                exit(0);
                break;
        }
    }
    displayMenu(renderer, pwadOptions, numPWADs, thirdpwadSelected, "Select 3rd optional PWAD (3 of 3):", &displayStart);
    SDL_Delay(35); // Moved displayMenu function call and added SDL_Delay to reduce unnecessary CPU usage - courtesy of @kloptops
}

    displayStart = 0;
// Map selection loop
initializeMapLabels();

int numMaps = MAX_EPISODES * MAX_MAPS_PER_EPISODE + MAX_D2_MAPS + 1;
int mapSelected = 0;
//int episode, map;

in_loop = true;
while (in_loop) {
    int key;

    while (1) {
        key = handleEvents();
        if (key == 0) {
            // No more events
            break;
        }

        if (key == SDLK_RETURN) {
            in_loop = false;
            break; // Exit loop on Enter key
        }

        switch (key) {
            case SDLK_UP:
                VAR_DEC(mapSelected, 1, numMaps);
                break;
            case SDLK_DOWN:
                VAR_INC(mapSelected, 1, numMaps);
                break;
            case SDLK_PAGEUP:
                mapSelected = max(0, mapSelected - SCROLL_SIZE);
                break;
            case SDLK_PAGEDOWN:
                mapSelected = min(numMaps - 1, mapSelected + SCROLL_SIZE);
                break;
            case SDLK_ESCAPE:
                SDL_Quit();
                exit(0);
                break;
        }
    }
    displayMenu(renderer, mapLabels, numMaps, mapSelected, "Level Warp ( e#m# = Doom, MAP## = Doom II ):", &displayStart);
    SDL_Delay(35); // Moved displayMenu function call and added SDL_Delay to reduce unnecessary CPU usage - courtesy of @kloptops
}


    displayStart = 0;
// Skill/Difficulty selection loop
displaySkillMenu();

int skillSelected = 0;
int numSkills = SKILL_LEVEL;

in_loop = true;
while(in_loop) { 
    int key;

    while (1) {
        key = handleEvents();
        if (key == 0) {
            // No more events
            break;
        }

        if (key == SDLK_RETURN) {
            in_loop = false;
            break; // Exit loop on Enter key
        }

        switch (key) {
            case SDLK_UP:
                    VAR_DEC(skillSelected, 1, SKILL_LEVEL);
                break;
            case SDLK_DOWN:
                    VAR_INC(skillSelected, 1, SKILL_LEVEL);
                break;
            case SDLK_ESCAPE:
                SDL_Quit();
                exit(0);
                break;
        }
    }
    displayMenu(renderer, skillLabels, numSkills, skillSelected, "Select Skill ( if using level warp ):", &displayStart);
    SDL_Delay(35); // Moved displayMenu function call and added SDL_Delay to reduce unnecessary CPU usage - courtesy of @kloptops
}


    displayStart = 0;
// Bot selection loop

int botSelected = 0;
int numBots = MAX_BOT_OPTIONS;

in_loop = true;
while(in_loop) { 
    int key;

    while (1) {
        key = handleEvents();
        if (key == 0) {
            // No more events
            break;
        }

        if (key == SDLK_RETURN) {
            in_loop = false;
            break; // Exit loop on Enter key
        }

        switch (key) {
            case SDLK_UP:
                VAR_DEC(botSelected, 1, numBots);
                break;
            case SDLK_DOWN:
                VAR_INC(botSelected, 1, numBots);
                break;
            case SDLK_PAGEUP:
                botSelected = max(0, botSelected - SCROLL_SIZE);
                break;
            case SDLK_PAGEDOWN:
                botSelected = min(numBots - 1, botSelected + SCROLL_SIZE);
                break;
            case SDLK_ESCAPE:
                SDL_Quit();
                exit(0);
                break;
        }
    }
    displayMenu(renderer, botOptions, numBots, botSelected, "Add Bots ( if using level warp ):", &displayStart);
    SDL_Delay(35);
}


// Display the command based on the selected source port
// strstr -> strcasestr courtesy of @kloptops

if (sourcePortSelected < numSourcePorts) {

        if (gp2keyb) {
            printf("%s %s & %s ", gp2keyb, files[sourcePortSelected], files[sourcePortSelected]);
        } else {
            printf("%s ", files[sourcePortSelected]);
        }

    // Additional parameters for gzDoom
        if (strcasestr(files[sourcePortSelected], gzDoomExecutable) != NULL) {
            printf("+gl_es 1 +vid_preferbackend 3 ");
        }

    // Display selected IWAD
        if (iwadSelected < numValidIWADs) {
            printf("-iwad %s ", validIWADs[iwadSelected]);
        }

    // Display selected PWADs
        if (firstpwadSelected != -1) {
            printf("-file \"%s\" ", pwadOptions[firstpwadSelected]);
        }

        if (secondpwadSelected != -1) {
            printf("-file \"%s\" ", pwadOptions[secondpwadSelected]);
        }

        if (thirdpwadSelected != -1) {
            printf("-file \"%s\" ", pwadOptions[thirdpwadSelected]);
        }

    // Display selected map
            printf("+map %s ", mapLabels[mapSelected]);
//        sscanf(mapLabels[mapSelected], "e%dm%d", &episode, &map);
//        if (episode > 0 && map > 0) {
//            printf("-warp %d %d ", episode, map);
//        } else {
//            sscanf(mapLabels[mapSelected], "MAP%02d", &map);
//            if (map > 0) {
//                printf("-warp %02d ", map);
//            }
//        }

    // Display skill level
        if (skillSelected != -1) {
            printf(" +set skill %d ", skillSelected);
        }

    // Cooperative with bots
        if (strstr(botOptions[botSelected], "Cooperative") != NULL) {
            numBots = atoi(&botOptions[botSelected][strlen("Cooperative, ")]);
            printf(" -cooperative +wait 42\\;");
            for (int i = 0; i < numBots; i++) {
                printf("addbot\\;");
            }
        }

    // Deathmatch with bots
        if (strstr(botOptions[botSelected], "Deathmatch") != NULL) {
            numBots = atoi(&botOptions[botSelected][strlen("Deathmatch, ")]);
            printf(" -nomonsters -deathmatch +wait 42\\;");
            for (int i = 0; i < numBots; i++) {
                printf("addbot\\;");
            }
        }

    printf("\n");
}

// Run the command
if (sourcePortSelected < numSourcePorts) {
    char command[4096]; // Adjust the size based on your needs

    // Append the source port
    if (gp2keyb) {
        snprintf(command, sizeof(command), "%s %s & %s ", gp2keyb, files[sourcePortSelected], files[sourcePortSelected]);
    } else {
        snprintf(command, sizeof(command), "%s ", files[sourcePortSelected]);
    }

    // Additional parameters for gzDoom
    // strstr -> strcasestr courtesy of @kloptops
        if (strcasestr(files[sourcePortSelected], gzDoomExecutable) != NULL) {
            snprintf(command + strlen(command), sizeof(command) - strlen(command), " +gl_es 1 +vid_preferbackend 3 ");
        }

    // Selected IWAD
        if (iwadSelected < numValidIWADs) {
            snprintf(command + strlen(command), sizeof(command) - strlen(command), " -iwad \"%s\"", validIWADs[iwadSelected]);
        }

    // Selected PWADs
        if (firstpwadSelected != -1) {
            snprintf(command + strlen(command), sizeof(command) - strlen(command), " -file \"%s\"", pwadOptions[firstpwadSelected]);
        }

        if (secondpwadSelected != -1) {
            snprintf(command + strlen(command), sizeof(command) - strlen(command), " -file \"%s\"", pwadOptions[secondpwadSelected]);
        }

        if (thirdpwadSelected != -1) {
            snprintf(command + strlen(command), sizeof(command) - strlen(command), " -file \"%s\"", pwadOptions[thirdpwadSelected]);
        }

    // Selected Map
            snprintf(command + strlen(command), sizeof(command) - strlen(command), " +map %s ", mapLabels[mapSelected]);
 //       sscanf(mapLabels[mapSelected], "e%dm%d", &episode, &map);
 //       if (episode > 0 && map > 0) {
 //           snprintf(command + strlen(command), sizeof(command) - strlen(command), " -warp %d %d ", episode, map);
 //       } else {
 //           sscanf(mapLabels[mapSelected], "MAP%02d", &map);
 //           if (map > 0) {
 //               snprintf(command + strlen(command), sizeof(command) - strlen(command), " -warp %02d ", map);
 //           }
 //       }

    // Selected Skill
        if (skillSelected != -1) {
            snprintf(command + strlen(command), sizeof(command) - strlen(command), " +set skill %d ", skillSelected);
        }

    // Cooperative with bots
        if (strstr(botOptions[botSelected], "Cooperative") != NULL) {
            numBots = atoi(&botOptions[botSelected][strlen("Cooperative, ")]);
            snprintf(command + strlen(command), sizeof(command) - strlen(command), " -cooperative +wait 42\\;");
            for (int i = 0; i < numBots; i++) {
                snprintf(command + strlen(command), sizeof(command) - strlen(command), "addbot\\;");
            }
        }

    // Deathmatch with bots
        if (strstr(botOptions[botSelected], "Deathmatch") != NULL) {
            numBots = atoi(&botOptions[botSelected][strlen("Deathmatch, ")]);
            snprintf(command + strlen(command), sizeof(command) - strlen(command), " -nomonsters -deathmatch +wait 42\\;");
            for (int i = 0; i < numBots; i++) {
                snprintf(command + strlen(command), sizeof(command) - strlen(command), "addbot\\;");
            }
        }

    // Print the final command string
        printf("\nCommand output:\n\n %s \n\n", command);

    // Run the command
        runCommand(command);

    // Cleanup and exit the program
        freeMemory(); // Free allocated memory
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(0);
}

freeMemory(); // Free allocated memory

return 0;
}
