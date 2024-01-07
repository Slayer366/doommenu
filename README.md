Doom Menu in SDL2
=============================

A Doom Menu written in SDL2.

## Dependencies
### Libraries
- SDL2
- SDL2_ttf

## Ubuntu / Debian
```
sudo apt install libsdl2-dev libsdl2-ttf-dev
```

## Compiling
```sh
git clone https://github.com/Slayer366/doommenu
cd doommenu
make
```

Add to roms/doom folder and launch with:
```
./doommenu
```

Can also be built to run on a "regular" Linux distro by commenting out the following lines before compiling:
```
//    system("pkill -f gptokeyb");
//    system("sudo kill -9 $(pidof gptokeyb)");
//    system("kill -9 $(pidof gptokeyb)");

    // Additional parameters for gzDoom
//        if (strcasestr(files[sourcePortSelected], gzDoomExecutable) != NULL) {
//            printf("+gl_es 1 +vid_preferbackend 3 ");
//        }

    // Additional parameters for gzDoom
//        if (strcasestr(files[sourcePortSelected], gzDoomExecutable) != NULL) {
//            snprintf(command + strlen(command), sizeof(command) - strlen(command), " +gl_es 1 +vid_preferbackend 3 ");
//        }
```
