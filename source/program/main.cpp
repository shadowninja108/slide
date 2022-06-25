#include <lib.hpp>
#include <nn.hpp>

#include "func_ptrs.hpp"

#include <math.h>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <atomic>
#include <stdlib.h>
#include <cstring>

constexpr float loopLow = 0;
constexpr float loopHigh = 2;
constexpr float loopStep = 0.005;

static double loop = 0;
static int player = 0;
static std::array<const char*, 3> playerNames = {
    "Mario",
    "Luigi",
    "Toad"
};

static sead::Color4f fromHsv(float h, float s, float v) {
    if(h > 360 || h <0 || s > 100 || s < 0 || v >100 || v <0){
        return { 1, 1, 1, 1 };
    }

    s /= 100;
    v /= 100;
    float c = s*v;
    float x = c*(1-abs(fmod(h/60.0, 2)-1));
    float m = v-c;
    float r,g,b;
    if(h >= 0 && h < 60){
        r = c,g = x,b = 0;
    }
    else if(h >= 60 && h < 120){
        r = x,g = c,b = 0;
    }
    else if(h >= 120 && h < 180){
        r = 0,g = c,b = x;
    }
    else if(h >= 180 && h < 240){
        r = 0,g = x,b = c;
    }
    else if(h >= 240 && h < 300){
        r = x,g = 0,b = c;
    }
    else{
        r = c,g = 0,b = x;
    }

    return {r+m, g+m, b+m, 1};
}


HOOK_DEFINE_TRAMPOLINE(DeathSignal) {
    static void Callback(uintptr_t _this) {

        player++;
        player %= playerNames.size();
        
        Orig(_this);
    }
};

HOOK_DEFINE_REPLACE(DbgTextWriterDraw2D) {
    static void Callback(uintptr_t _this, agl::lyr::RenderInfo const& renderInfo) {
        loop += loopStep;
        if(loop > loopHigh)
            loop = loopLow;

        DbgTextWriter::Info info;

        info.mLayerId = renderInfo.mLayerId;
        info.mPos = {0, -200};
        info.mEx.mScale = {3,3};
        info.mEx.mColor = fromHsv(((loop - loopLow) / (loopHigh - loopLow)) * 360, 100, 100);

        snprintf(info.mStrUtf8Data, sizeof(info.mStrUtf8Data), "Current player: %s", playerNames.at(player));
        info.mEx.mFlags |= 0x1 | 0x400;

        Lp_Sys_DbgTextWriter_drawText(_this, renderInfo, &info);
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    envSetOwnProcessHandle(exl::util::proc_handle::Get());
    exl::hook::Initialize();

    namespace armv8 = exl::armv8;
    namespace reg = armv8::reg;
    namespace inst = armv8::inst;

    DbgTextWriterDraw2D::InstallAtFuncPtr(Lp_Sys_DbgTextWriter_draw2D);
    DeathSignal::InstallAtOffset(0x015b47b0);
}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}