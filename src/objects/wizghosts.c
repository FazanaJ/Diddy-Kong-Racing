#include "src/object_functions.h"

#include "src/audio.h"
#include "src/audio_spatial.h"
#include "src/audio_vehicle.h"
#include "src/audiosfx.h"
#include "src/camera.h"
#include "src/collision.h"
#include "common.h"
#include "src/fade_transition.h"
#include "src/font.h"
#include "src/game.h"
#include "src/game_text.h"
#include "src/game_ui.h"
#include "src/joypad.h"
#include "src/lights.h"
#include "macros.h"
#include "src/math_util.h"
#include "src/menu.h" // For cheats
#include "src/object_models.h"
#include "src/objects.h"
#include "src/particles.h"
#include "PR/os_cont.h"
#include "PRinternal/viint.h"
#include "src/printf.h"
#include "src/racer.h"
#include "structs.h"
#include "src/textures_sprites.h"
#include "src/thread3_main.h"
#include "src/tracks.h"
#include "types.h"
#include "src/video.h"
#include "src/waves.h"
#include "src/weather.h"

// GLOBAL_ASM

void obj_loop_wizghosts(Object *obj, s32 updateRate) {
    func_8001F460(obj, updateRate, obj);
    obj->animFrame = (obj->animFrame + (updateRate * 8)) & 0xFF;
}
