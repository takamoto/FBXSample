#pragma once

#if defined(SHADER_LEVEL_5_0)

#define VERTEX_SHADER_PROFILE  "vs_5_0"
#define PIXEL_SHADER_PROFILE  "ps_5_0"
#define GEOMETORY_SHADER_PROFILE  "gs_5_0"

#elif defined(SHADER_LEVEL_4_1)

#define VERTEX_SHADER_PROFILE  "vs_4_1"
#define PIXEL_SHADER_PROFILE  "ps_4_1"
#define GEOMETORY_SHADER_PROFILE  "gs_4_1"

#elif defined(SHADER_LEVEL_4_0)

#define VERTEX_SHADER_PROFILE  "vs_4_0"
#define PIXEL_SHADER_PROFILE  "ps_4_0"
#define GEOMETORY_SHADER_PROFILE  "gs_4_0"

#else

static_assert(false, "SHADER_LEVEL is not defined.");

#endif