
#include "replay.h"

#include <stdio.h>
#if defined(_WIN32)
    #include <conio.h>
#else
    // Required for kbhit() function in non-Windows platforms
    #include <stdio.h>
    #include <termios.h>
    #include <unistd.h>
    #include <fcntl.h>
#endif

#define KEY_ESCAPE  27

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
#if !defined(_WIN32)
static int kbhit(void);         // Check if a key has been pressed
static char getch();            // Get pressed character
#else
#define kbhit _kbhit
#define getch _getch
#endif

int main(int argc, char *argv[])
{
    static unsigned char key = 0;
    replay_InitAudioDevice();

    RSound fxWav = replay_LoadSound("resources/weird.wav");
    RSound fxOgg = replay_LoadSound("resources/target.ogg");

    RMusic music = replay_LoadMusic("resources/country.mp3");
    replay_PlayMusic(music);

    printf("\nPress s or d to play sounds, ESC to stop...\n");
    while (key != KEY_ESCAPE)
    {
        if (kbhit()) key = getch();
        if ((key == 's') || (key == 'S')) replay_PlaySound(fxWav);
        if ((key == 'd') || (key == 'D')) replay_PlaySound(fxOgg);
        key = 0;
        replay_UpdateMusic(music);
    }

    replay_UnloadSound(fxWav);
    replay_UnloadSound(fxOgg);
    replay_UnloadMusic(music);
    replay_CloseAudioDevice();

    return 1;
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
#if !defined(_WIN32)
// Check if a key has been pressed
static int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

// Get pressed character
static char getch() { return getchar(); }
#endif
