#pragma once

#ifdef INDEX_UINT16
#define INDEX_FORMAT_ENUM DXGI_FORMAT_R16_UINT
#define INDEX_FORMAT_TYPE uint16_t
#elif INDEX_UINT32
#define INDEX_FORMAT_ENUM DXGI_FORMAT_R32_UINT
#define INDEX_FORMAT_TYPE uint32_t
#else
static_assert(false, "DXGI_FORMAT is not defined.");
#endif