#ifndef _AUTOPLAY_H_
#define _AUTOPLAY_H_

typedef enum AutoPlayTypes {
    AUTOPLAY_OFF,   // Don't use autoplay
    AUTOPLAY_SINGLE_PLAYER, // Autoplay will play through most of single player
    AUTOPLAY_TRACKS_MODE,   // Autoplay will first test each track loads in tracks mode, then play through each track
    AUTOPLAY_TIME_TRIAL,   // Autoplay will play through each course in time trial mode.
    AUTOPLAY_2_PLAYER,  // Autoplay will play through each track with 2 players
    AUTOPLAY_3_PLAYER,  // Autoplay will play through each track with 3 players
    AUTOPLAY_4_PLAYER,  // Autoplay will play through each track with 4 players
    AUTOPLAY_OPTIONS,   // Autoplay will go through the options menu and ensure things don't explode

    AUTOPLAY_COUNT
} AutoPlayTypes;

typedef enum AutoPlayTracks {
    AUTOPLAY_TRACKS_PREVIEW,
    AUTOPLAY_TRACKS_RESET,
    AUTOPLAY_TRACKS_PLAY,
    AUTOPLAY_TRACKS_FINISHED,
} AutoPlayTracks;

extern u8 gAutoplayTest;

s32 autoplay_drive(f32 x, f32 y, f32 z, f32 distCheck);
Object *autoplay_find_balloon(s32 balloonID);
Object *autoplay_find_balloon2(f32 x, f32 z);
void autoplay_inputs(void);

#endif
