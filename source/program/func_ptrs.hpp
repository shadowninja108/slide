#pragma once

#include <lib.hpp>

#define _FUNC_PREFIX(name)  \
    slide::func_ptrs::name

#define _FUNC_TYPE(name)    \
    APPEND(_FUNC_PREFIX(name), _t)

#define _FUNC_OFFSET(name)  \
    APPEND(_FUNC_PREFIX(name), _Offset)

#define FUNC_PTR(offset, name, ...)                             \
    namespace slide::func_ptrs {                                \
        using APPEND(name, _t) = __VA_ARGS__;                   \
        static constexpr size_t APPEND(name, _Offset) = offset; \
    }                                                           \
    extern _FUNC_TYPE(name) name

#define SETUP_FUNC_PTR(name)     \
    _FUNC_TYPE(name) name = reinterpret_cast<_FUNC_TYPE(name)>(exl::util::modules::GetTargetOffset(_FUNC_OFFSET(name)))


#include "misc_types.hpp"
#include <sead/gfx/seadTextWriter.h>
#include <sead/gfx/seadDrawContext.h>
#include <sead/fake/seadPrimitiveDrawer.hpp>

FUNC_PTR(0x0062E140, Lp_Sys_DbgTextWriter_draw2D, void(*)(uintptr_t, agl::lyr::RenderInfo const&));
FUNC_PTR(0x0062DC60, Lp_Sys_DbgTextWriter_drawText, void(*)(uintptr_t, agl::lyr::RenderInfo const&, DbgTextWriter::Info*));

#undef FUNC_PTR
