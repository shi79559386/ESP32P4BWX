/*******************************************************************************
 * Size: 28 px
 * Bpp: 4
 * Opts: --bpp 4 --size 28 --no-compress --font digitaldreamfatnarrow.ttf --symbols 0123456789.°C% --format lvgl -o lv_font_digital_28.c
 ******************************************************************************/
#include "lv_font_digital_28.h"
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef LV_FONT_DIGITAL_28
#define LV_FONT_DIGITAL_28 1
#endif

#if LV_FONT_DIGITAL_28

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0025 "%" */
    0x0, 0x9f, 0xff, 0xf8, 0x0, 0x0, 0x0, 0x0,
    0x6, 0xff, 0xff, 0xff, 0x50, 0x0, 0x0, 0x0,
    0x2f, 0xff, 0xff, 0xff, 0xe2, 0x0, 0x24, 0x0,
    0x8f, 0xf5, 0x44, 0x6f, 0xf7, 0x0, 0xcc, 0x0,
    0x8f, 0xf0, 0x0, 0x2f, 0xf7, 0x5, 0xfc, 0x0,
    0x8f, 0xf0, 0x0, 0x2f, 0xf7, 0xd, 0xfc, 0x0,
    0x8f, 0xf0, 0x0, 0x2f, 0xf7, 0x6f, 0xf9, 0x0,
    0x8f, 0xf0, 0x0, 0x2f, 0xf8, 0xef, 0xf2, 0x0,
    0x8f, 0xf0, 0x0, 0x2f, 0xfe, 0xff, 0x90, 0x0,
    0x8f, 0xf0, 0x0, 0x2f, 0xff, 0xff, 0x10, 0x0,
    0x8f, 0xf0, 0x0, 0x2f, 0xff, 0xf8, 0x0, 0x0,
    0x8f, 0xf0, 0x0, 0x2f, 0xff, 0xe0, 0x0, 0x0,
    0x5f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0x0,
    0xa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb0,
    0x0, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf6,
    0x0, 0x15, 0x5b, 0xff, 0xfa, 0x55, 0x5d, 0xfc,
    0x0, 0x0, 0x1f, 0xff, 0xf7, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x9f, 0xff, 0xf7, 0x0, 0xc, 0xfd,
    0x0, 0x2, 0xff, 0xff, 0xf7, 0x0, 0xc, 0xfd,
    0x0, 0xb, 0xff, 0x8f, 0xf7, 0x0, 0xc, 0xfd,
    0x0, 0x4f, 0xfc, 0x2f, 0xf7, 0x0, 0xc, 0xfd,
    0x0, 0x7f, 0xf3, 0x2f, 0xf7, 0x0, 0xc, 0xfd,
    0x0, 0x7f, 0xa0, 0x2f, 0xf7, 0x0, 0xc, 0xfd,
    0x0, 0x7f, 0x20, 0x2f, 0xfa, 0x44, 0x4d, 0xfc,
    0x0, 0x24, 0x0, 0xc, 0xff, 0xff, 0xff, 0xf7,
    0x0, 0x0, 0x0, 0x1, 0xef, 0xff, 0xff, 0xb0,
    0x0, 0x0, 0x0, 0x0, 0x3f, 0xff, 0xfd, 0x0,

    /* U+002E "." */
    0x0, 0x0, 0x1d, 0xf3, 0x6f, 0xfa, 0x5f, 0xfa,
    0xc, 0xe3,

    /* U+0030 "0" */
    0x0, 0x9f, 0xff, 0xf8, 0x3f, 0xff, 0xfd, 0x0,
    0x6, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb0,
    0x2f, 0xff, 0xff, 0xfb, 0x7f, 0xff, 0xff, 0xf7,
    0x8f, 0xf5, 0x44, 0x40, 0x4, 0x44, 0xef, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x6, 0xff, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0xe, 0xff, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x7f, 0xff, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x1, 0xef, 0xfd, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x8, 0xff, 0x8c, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0xf, 0xfe, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x1f, 0xf7, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x1f, 0xd0, 0xc, 0xfd,
    0x4f, 0xc0, 0x0, 0x0, 0x8, 0x40, 0x7, 0xf9,
    0xb, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe0,
    0x4f, 0xb0, 0x1, 0x83, 0x0, 0x0, 0x7, 0xf8,
    0x8f, 0xf0, 0x9, 0xf6, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x2f, 0xf6, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0xaf, 0xf5, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf4, 0xff, 0xd0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xfd, 0xff, 0x40, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xff, 0xfc, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xff, 0xf3, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xff, 0xa0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xff, 0x64, 0x40, 0x4, 0x44, 0x4d, 0xfc,
    0x2f, 0xff, 0xff, 0xfb, 0x7f, 0xff, 0xff, 0xf7,
    0x6, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb0,
    0x0, 0x9f, 0xff, 0xf7, 0x3f, 0xff, 0xfd, 0x0,

    /* U+0031 "1" */
    0x8, 0xff, 0xff, 0x80, 0x0, 0x0, 0x0, 0x4f,
    0xff, 0xff, 0xf5, 0x0, 0x0, 0x0, 0xc, 0xff,
    0xff, 0xfe, 0x10, 0x0, 0x0, 0x1, 0x44, 0x46,
    0xff, 0x60, 0x0, 0x0, 0x0, 0x0, 0x2, 0xff,
    0x70, 0x0, 0x0, 0x0, 0x0, 0x2, 0xff, 0x70,
    0x0, 0x0, 0x0, 0x0, 0x2, 0xff, 0x70, 0x0,
    0x0, 0x0, 0x0, 0x2, 0xff, 0x70, 0x0, 0x0,
    0x0, 0x0, 0x2, 0xff, 0x70, 0x0, 0x0, 0x0,
    0x0, 0x2, 0xff, 0x70, 0x0, 0x0, 0x0, 0x0,
    0x2, 0xff, 0x70, 0x0, 0x0, 0x0, 0x0, 0x2,
    0xff, 0x70, 0x0, 0x0, 0x0, 0x0, 0x0, 0xdf,
    0x30, 0x0, 0x0, 0x0, 0x0, 0x0, 0x49, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0xdf, 0x20, 0x0,
    0x0, 0x0, 0x0, 0x2, 0xff, 0x70, 0x0, 0x0,
    0x0, 0x0, 0x2, 0xff, 0x70, 0x0, 0x0, 0x0,
    0x0, 0x2, 0xff, 0x70, 0x0, 0x0, 0x0, 0x0,
    0x2, 0xff, 0x70, 0x0, 0x0, 0x0, 0x0, 0x2,
    0xff, 0x70, 0x0, 0x0, 0x0, 0x0, 0x2, 0xff,
    0x70, 0x0, 0x0, 0x0, 0x0, 0x2, 0xff, 0x70,
    0x0, 0x0, 0x0, 0x0, 0x2, 0xff, 0x70, 0x0,
    0x0, 0x1, 0x44, 0x46, 0xff, 0x94, 0x44, 0x20,
    0xc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe2, 0x4f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x8, 0xff,
    0xff, 0x73, 0xff, 0xff, 0xc0,

    /* U+0032 "2" */
    0x0, 0x9f, 0xff, 0xf8, 0x3f, 0xff, 0xfd, 0x0,
    0x5, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb0,
    0x0, 0xdf, 0xff, 0xfb, 0x7f, 0xff, 0xff, 0xf7,
    0x0, 0x14, 0x44, 0x40, 0x4, 0x44, 0x4d, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x9f, 0xff, 0xf7, 0x3f, 0xff, 0xff, 0xfa,
    0x6, 0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xe1,
    0x2f, 0xff, 0xff, 0xfb, 0x7f, 0xff, 0xff, 0x20,
    0x8f, 0xf5, 0x55, 0x51, 0x4, 0x55, 0x53, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8f, 0xf5, 0x44, 0x40, 0x4, 0x44, 0x42, 0x0,
    0x2f, 0xff, 0xff, 0xfb, 0x7f, 0xff, 0xff, 0x30,
    0x6, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x90,
    0x0, 0x9f, 0xff, 0xf7, 0x3f, 0xff, 0xfd, 0x0,

    /* U+0033 "3" */
    0x8, 0xff, 0xff, 0x83, 0xff, 0xff, 0xd0, 0x4,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb0, 0xc,
    0xff, 0xff, 0xb7, 0xff, 0xff, 0xff, 0x70, 0x14,
    0x44, 0x41, 0x4, 0x44, 0x4d, 0xfd, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0xcf, 0xd0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0xc, 0xfd, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0xcf, 0xd0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0xc, 0xfd, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0xcf, 0xd0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0xc, 0xfd, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0xcf, 0xd0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0xc, 0xfd, 0x0, 0x0, 0x0, 0x3, 0xff, 0xff,
    0xff, 0xa0, 0x0, 0x0, 0x0, 0xef, 0xff, 0xff,
    0xf1, 0x0, 0x0, 0x0, 0x7, 0xff, 0xff, 0xff,
    0x60, 0x0, 0x0, 0x0, 0x4, 0x55, 0x5d, 0xfc,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xcf, 0xd0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0xcf, 0xd0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0xc, 0xfd, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0xcf, 0xd0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0xc, 0xfd, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0xcf, 0xd0, 0x14, 0x44, 0x41,
    0x4, 0x44, 0x4d, 0xfc, 0xb, 0xff, 0xff, 0xc7,
    0xff, 0xff, 0xff, 0x74, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xb0, 0x8, 0xff, 0xff, 0x83, 0xff,
    0xff, 0xd0, 0x0,

    /* U+0034 "4" */
    0x6, 0x10, 0x0, 0x0, 0x0, 0x0, 0x0, 0x80,
    0x3f, 0xb0, 0x0, 0x0, 0x0, 0x0, 0x7, 0xf8,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf4, 0x44, 0x40, 0x3, 0x44, 0x4d, 0xfd,
    0x3f, 0xff, 0xff, 0xfb, 0x6f, 0xff, 0xff, 0xf7,
    0x7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0,
    0x0, 0x9f, 0xff, 0xf7, 0x3f, 0xff, 0xff, 0xf9,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xb, 0xfc,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3, 0xf4,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x30,

    /* U+0035 "5" */
    0x0, 0x9f, 0xff, 0xf8, 0x3f, 0xff, 0xfd, 0x0,
    0x6, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x90,
    0x2f, 0xff, 0xff, 0xfb, 0x7f, 0xff, 0xff, 0x20,
    0x8f, 0xf5, 0x44, 0x40, 0x4, 0x44, 0x42, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x5f, 0xff, 0xff, 0xf7, 0x3f, 0xff, 0xfd, 0x0,
    0xa, 0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xb0,
    0x0, 0xdf, 0xff, 0xfb, 0x7f, 0xff, 0xff, 0xf6,
    0x0, 0x15, 0x55, 0x51, 0x4, 0x55, 0x5d, 0xfc,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x14, 0x44, 0x40, 0x4, 0x44, 0x4d, 0xfc,
    0x0, 0xcf, 0xff, 0xfb, 0x7f, 0xff, 0xff, 0xf7,
    0x5, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb0,
    0x0, 0x9f, 0xff, 0xf7, 0x3f, 0xff, 0xfd, 0x0,

    /* U+0036 "6" */
    0x0, 0x9f, 0xff, 0xf8, 0x3f, 0xff, 0xfd, 0x0,
    0x6, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x90,
    0x2f, 0xff, 0xff, 0xfb, 0x7f, 0xff, 0xff, 0x20,
    0x8f, 0xf5, 0x44, 0x40, 0x4, 0x44, 0x42, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x5f, 0xff, 0xff, 0xf7, 0x3f, 0xff, 0xfd, 0x0,
    0xb, 0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xb0,
    0x2f, 0xff, 0xff, 0xfb, 0x7f, 0xff, 0xff, 0xf6,
    0x8f, 0xf5, 0x55, 0x51, 0x4, 0x55, 0x5d, 0xfc,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf5, 0x44, 0x40, 0x4, 0x44, 0x4d, 0xfc,
    0x2f, 0xff, 0xff, 0xfb, 0x7f, 0xff, 0xff, 0xf7,
    0x6, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb0,
    0x0, 0x9f, 0xff, 0xf7, 0x3f, 0xff, 0xfd, 0x0,

    /* U+0037 "7" */
    0x8, 0xff, 0xff, 0x83, 0xff, 0xff, 0xc0, 0x4f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0xc, 0xff,
    0xff, 0xb7, 0xff, 0xff, 0xe2, 0x1, 0x44, 0x44,
    0x0, 0x44, 0x5f, 0xb0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x6f, 0xb0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0xef, 0xb0, 0x0, 0x0, 0x0, 0x0, 0x8, 0xff,
    0x80, 0x0, 0x0, 0x0, 0x0, 0x1f, 0xff, 0x10,
    0x0, 0x0, 0x0, 0x0, 0x9f, 0xf7, 0x0, 0x0,
    0x0, 0x0, 0x0, 0xff, 0xe0, 0x0, 0x0, 0x0,
    0x0, 0x1, 0xff, 0x60, 0x0, 0x0, 0x0, 0x0,
    0x1, 0xfd, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x73, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x19, 0x30, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x9f, 0x60, 0x0, 0x0, 0x0,
    0x0, 0x2, 0xff, 0x60, 0x0, 0x0, 0x0, 0x0,
    0xa, 0xff, 0x50, 0x0, 0x0, 0x0, 0x0, 0x3f,
    0xfd, 0x0, 0x0, 0x0, 0x0, 0x0, 0xcf, 0xf4,
    0x0, 0x0, 0x0, 0x0, 0x4, 0xff, 0xb0, 0x0,
    0x0, 0x0, 0x0, 0x6, 0xff, 0x30, 0x0, 0x0,
    0x0, 0x0, 0x6, 0xfa, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x6, 0xf2, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x2, 0x30, 0x0, 0x0, 0x0, 0x0, 0x0,

    /* U+0038 "8" */
    0x0, 0x9f, 0xff, 0xf8, 0x3f, 0xff, 0xfd, 0x0,
    0x6, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb0,
    0x2f, 0xff, 0xff, 0xfb, 0x7f, 0xff, 0xff, 0xf7,
    0x8f, 0xf5, 0x44, 0x40, 0x4, 0x44, 0x4d, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x5f, 0xff, 0xff, 0xf7, 0x3f, 0xff, 0xff, 0xfa,
    0xb, 0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xf1,
    0x2f, 0xff, 0xff, 0xfb, 0x7f, 0xff, 0xff, 0xf6,
    0x8f, 0xf5, 0x55, 0x51, 0x4, 0x55, 0x5d, 0xfc,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf5, 0x44, 0x40, 0x4, 0x44, 0x4d, 0xfc,
    0x2f, 0xff, 0xff, 0xfb, 0x7f, 0xff, 0xff, 0xf7,
    0x6, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb0,
    0x0, 0x9f, 0xff, 0xf7, 0x3f, 0xff, 0xfd, 0x0,

    /* U+0039 "9" */
    0x0, 0x9f, 0xff, 0xf8, 0x3f, 0xff, 0xfd, 0x0,
    0x6, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb0,
    0x2f, 0xff, 0xff, 0xfb, 0x7f, 0xff, 0xff, 0xf7,
    0x8f, 0xf5, 0x44, 0x40, 0x4, 0x44, 0x4d, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x5f, 0xff, 0xff, 0xf7, 0x3f, 0xff, 0xff, 0xfa,
    0xa, 0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xf1,
    0x0, 0xdf, 0xff, 0xfb, 0x7f, 0xff, 0xff, 0xf6,
    0x0, 0x15, 0x55, 0x51, 0x4, 0x55, 0x5d, 0xfc,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xfd,
    0x0, 0x14, 0x44, 0x40, 0x4, 0x44, 0x4d, 0xfc,
    0x0, 0xcf, 0xff, 0xfb, 0x7f, 0xff, 0xff, 0xf7,
    0x5, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb0,
    0x0, 0x9f, 0xff, 0xf7, 0x3f, 0xff, 0xfd, 0x0,

    /* U+0043 "C" */
    0x0, 0x9f, 0xff, 0xf7, 0x4f, 0xff, 0xfc, 0x0,
    0x6f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x2f,
    0xff, 0xff, 0xfb, 0x8f, 0xff, 0xfe, 0x28, 0xff,
    0x54, 0x44, 0x0, 0x44, 0x44, 0x20, 0x8f, 0xf0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x8, 0xff, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x8f, 0xf0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x8, 0xff, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x8f, 0xf0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x8, 0xff, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x8f, 0xf0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x8, 0xff, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x4f, 0xc0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0xb3, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x4f, 0xb0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8f, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x8,
    0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x8f,
    0xf0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x8, 0xff,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x8f, 0xf0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x8, 0xff, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x8f, 0xf0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x8, 0xff, 0x54, 0x44,
    0x0, 0x44, 0x44, 0x20, 0x2f, 0xff, 0xff, 0xfb,
    0x8f, 0xff, 0xfe, 0x20, 0x6f, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xf8, 0x0, 0x9f, 0xff, 0xf7, 0x4f,
    0xff, 0xfc, 0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 287, .box_w = 16, .box_h = 27, .ofs_x = 0, .ofs_y = -5},
    {.bitmap_index = 216, .adv_w = 287, .box_w = 4, .box_h = 5, .ofs_x = 6, .ofs_y = -5},
    {.bitmap_index = 226, .adv_w = 287, .box_w = 16, .box_h = 27, .ofs_x = 0, .ofs_y = -5},
    {.bitmap_index = 442, .adv_w = 287, .box_w = 14, .box_h = 27, .ofs_x = 1, .ofs_y = -5},
    {.bitmap_index = 631, .adv_w = 287, .box_w = 16, .box_h = 27, .ofs_x = 0, .ofs_y = -5},
    {.bitmap_index = 847, .adv_w = 287, .box_w = 15, .box_h = 27, .ofs_x = 1, .ofs_y = -5},
    {.bitmap_index = 1050, .adv_w = 287, .box_w = 16, .box_h = 25, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1250, .adv_w = 287, .box_w = 16, .box_h = 27, .ofs_x = 0, .ofs_y = -5},
    {.bitmap_index = 1466, .adv_w = 287, .box_w = 16, .box_h = 27, .ofs_x = 0, .ofs_y = -5},
    {.bitmap_index = 1682, .adv_w = 287, .box_w = 14, .box_h = 25, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1857, .adv_w = 287, .box_w = 16, .box_h = 27, .ofs_x = 0, .ofs_y = -5},
    {.bitmap_index = 2073, .adv_w = 287, .box_w = 16, .box_h = 27, .ofs_x = 0, .ofs_y = -5},
    {.bitmap_index = 2289, .adv_w = 287, .box_w = 15, .box_h = 27, .ofs_x = 0, .ofs_y = -5}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0x9, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
    0x11, 0x12, 0x13, 0x14, 0x1e
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 37, .range_length = 31, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 13, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 4,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t lv_font_digital_28 = {
#else
lv_font_t lv_font_digital_28 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 27,          /*The maximum line height required by the font*/
    .base_line = 5,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -4,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if LV_FONT_DIGITAL_28*/

