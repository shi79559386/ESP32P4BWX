/*******************************************************************************
 * Size: 18 px
 * Bpp: 4
 * Opts: --bpp 4 --size 18 --no-compress --font digitaldreamfatnarrow.ttf --symbols 0123456789C --format lvgl -o lv_font_digital_18.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef LV_FONT_DIGITAL_18
#define LV_FONT_DIGITAL_18 1
#endif

#if LV_FONT_DIGITAL_18

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0030 "0" */
    0x9, 0xff, 0xe8, 0xff, 0xe2, 0x5, 0xff, 0xfe,
    0xaf, 0xff, 0xd0, 0xbf, 0x11, 0x0, 0x16, 0xff,
    0x2b, 0xf0, 0x0, 0x0, 0xdf, 0xf2, 0xbf, 0x0,
    0x0, 0x7f, 0xff, 0x2b, 0xf0, 0x0, 0x1e, 0xe8,
    0xf2, 0xbf, 0x0, 0x4, 0xf6, 0x7f, 0x2a, 0xe0,
    0x0, 0x4c, 0x7, 0xf2, 0x38, 0x0, 0x0, 0x0,
    0x1a, 0xa, 0xe0, 0x5a, 0x0, 0x6, 0xf2, 0xbf,
    0xe, 0xc0, 0x0, 0x7f, 0x2b, 0xf7, 0xf7, 0x0,
    0x7, 0xf2, 0xbf, 0xfe, 0x0, 0x0, 0x7f, 0x2b,
    0xff, 0x50, 0x0, 0x7, 0xf2, 0xbf, 0xd1, 0x0,
    0x11, 0x8f, 0x25, 0xff, 0xfe, 0xaf, 0xff, 0xd0,
    0x9, 0xff, 0xe9, 0xff, 0xe2, 0x0,

    /* U+0031 "1" */
    0x9, 0xff, 0xe1, 0x0, 0x0, 0xa, 0xff, 0xfb,
    0x0, 0x0, 0x0, 0x11, 0xaf, 0x10, 0x0, 0x0,
    0x0, 0x9f, 0x10, 0x0, 0x0, 0x0, 0x9f, 0x10,
    0x0, 0x0, 0x0, 0x9f, 0x10, 0x0, 0x0, 0x0,
    0x9f, 0x10, 0x0, 0x0, 0x0, 0x8f, 0x0, 0x0,
    0x0, 0x0, 0x29, 0x0, 0x0, 0x0, 0x0, 0x8f,
    0x0, 0x0, 0x0, 0x0, 0x9f, 0x10, 0x0, 0x0,
    0x0, 0x9f, 0x10, 0x0, 0x0, 0x0, 0x9f, 0x10,
    0x0, 0x0, 0x0, 0x9f, 0x10, 0x0, 0x0, 0x11,
    0xaf, 0x21, 0x0, 0xa, 0xff, 0xff, 0xff, 0xf2,
    0x9, 0xff, 0xe9, 0xff, 0xe1,

    /* U+0032 "2" */
    0x9, 0xff, 0xe8, 0xff, 0xe2, 0x0, 0xbf, 0xfe,
    0xaf, 0xff, 0xd0, 0x0, 0x11, 0x0, 0x11, 0x8f,
    0x20, 0x0, 0x0, 0x0, 0x7, 0xf2, 0x0, 0x0,
    0x0, 0x0, 0x7f, 0x20, 0x0, 0x0, 0x0, 0x7,
    0xf2, 0x0, 0x0, 0x0, 0x0, 0x7f, 0x20, 0x9f,
    0xfe, 0x8f, 0xff, 0xe0, 0x5f, 0xff, 0xea, 0xff,
    0xf3, 0xb, 0xf1, 0x11, 0x1, 0x11, 0x0, 0xbf,
    0x0, 0x0, 0x0, 0x0, 0xb, 0xf0, 0x0, 0x0,
    0x0, 0x0, 0xbf, 0x0, 0x0, 0x0, 0x0, 0xb,
    0xf0, 0x0, 0x0, 0x0, 0x0, 0xbf, 0x11, 0x0,
    0x11, 0x10, 0x5, 0xff, 0xfe, 0xaf, 0xff, 0x30,
    0x9, 0xff, 0xe9, 0xff, 0xe2, 0x0,

    /* U+0033 "3" */
    0x8, 0xff, 0xe8, 0xff, 0xe2, 0x0, 0xaf, 0xfe,
    0xaf, 0xff, 0xd0, 0x0, 0x11, 0x0, 0x11, 0x8f,
    0x20, 0x0, 0x0, 0x0, 0x7, 0xf2, 0x0, 0x0,
    0x0, 0x0, 0x7f, 0x20, 0x0, 0x0, 0x0, 0x7,
    0xf2, 0x0, 0x0, 0x0, 0x0, 0x7f, 0x20, 0x0,
    0x0, 0x7f, 0xff, 0xe0, 0x0, 0x0, 0x9, 0xff,
    0xfd, 0x0, 0x0, 0x0, 0x1, 0x18, 0xf2, 0x0,
    0x0, 0x0, 0x0, 0x7f, 0x20, 0x0, 0x0, 0x0,
    0x7, 0xf2, 0x0, 0x0, 0x0, 0x0, 0x7f, 0x20,
    0x0, 0x0, 0x0, 0x7, 0xf2, 0x0, 0x11, 0x0,
    0x11, 0x8f, 0x20, 0xaf, 0xfe, 0xaf, 0xff, 0xd0,
    0x9, 0xff, 0xe8, 0xff, 0xe2, 0x0,

    /* U+0034 "4" */
    0x24, 0x0, 0x0, 0x0, 0x7, 0xa, 0xe0, 0x0,
    0x0, 0x7, 0xf2, 0xbf, 0x0, 0x0, 0x0, 0x7f,
    0x2b, 0xf0, 0x0, 0x0, 0x7, 0xf2, 0xbf, 0x0,
    0x0, 0x0, 0x7f, 0x2b, 0xf0, 0x0, 0x0, 0x7,
    0xf2, 0xbf, 0x11, 0x0, 0x11, 0x8f, 0x26, 0xff,
    0xfe, 0xaf, 0xff, 0xd0, 0x9, 0xff, 0xe9, 0xff,
    0xfd, 0x0, 0x0, 0x0, 0x0, 0x7, 0xf2, 0x0,
    0x0, 0x0, 0x0, 0x7f, 0x20, 0x0, 0x0, 0x0,
    0x7, 0xf2, 0x0, 0x0, 0x0, 0x0, 0x7f, 0x20,
    0x0, 0x0, 0x0, 0x7, 0xf2, 0x0, 0x0, 0x0,
    0x0, 0x6f, 0x10, 0x0, 0x0, 0x0, 0x0, 0x50,

    /* U+0035 "5" */
    0x9, 0xff, 0xe8, 0xff, 0xe2, 0x5, 0xff, 0xfe,
    0xaf, 0xff, 0x30, 0xbf, 0x11, 0x0, 0x11, 0x10,
    0xb, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xbf, 0x0,
    0x0, 0x0, 0x0, 0xb, 0xf0, 0x0, 0x0, 0x0,
    0x0, 0xbf, 0x0, 0x0, 0x0, 0x0, 0x7, 0xff,
    0xfe, 0x8f, 0xfe, 0x20, 0xb, 0xff, 0xea, 0xff,
    0xfc, 0x0, 0x1, 0x11, 0x1, 0x18, 0xf2, 0x0,
    0x0, 0x0, 0x0, 0x7f, 0x20, 0x0, 0x0, 0x0,
    0x7, 0xf2, 0x0, 0x0, 0x0, 0x0, 0x7f, 0x20,
    0x0, 0x0, 0x0, 0x7, 0xf2, 0x0, 0x11, 0x0,
    0x11, 0x8f, 0x20, 0xaf, 0xfe, 0xaf, 0xff, 0xd0,
    0x9, 0xff, 0xe9, 0xff, 0xe2, 0x0,

    /* U+0036 "6" */
    0x9, 0xff, 0xe8, 0xff, 0xe2, 0x5, 0xff, 0xfe,
    0xaf, 0xff, 0x30, 0xbf, 0x11, 0x0, 0x11, 0x10,
    0xb, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xbf, 0x0,
    0x0, 0x0, 0x0, 0xb, 0xf0, 0x0, 0x0, 0x0,
    0x0, 0xbf, 0x0, 0x0, 0x0, 0x0, 0x7, 0xff,
    0xfe, 0x8f, 0xfe, 0x20, 0x6f, 0xff, 0xea, 0xff,
    0xfc, 0xb, 0xf1, 0x11, 0x1, 0x18, 0xf2, 0xbf,
    0x0, 0x0, 0x0, 0x7f, 0x2b, 0xf0, 0x0, 0x0,
    0x7, 0xf2, 0xbf, 0x0, 0x0, 0x0, 0x7f, 0x2b,
    0xf0, 0x0, 0x0, 0x7, 0xf2, 0xbf, 0x11, 0x0,
    0x11, 0x8f, 0x25, 0xff, 0xfe, 0xaf, 0xff, 0xd0,
    0x9, 0xff, 0xe9, 0xff, 0xe2, 0x0,

    /* U+0037 "7" */
    0x9, 0xff, 0xe8, 0xff, 0xe2, 0xa, 0xff, 0xea,
    0xff, 0xf2, 0x0, 0x11, 0x0, 0x17, 0xd0, 0x0,
    0x0, 0x0, 0xd, 0xc0, 0x0, 0x0, 0x0, 0x7f,
    0x80, 0x0, 0x0, 0x0, 0xee, 0x0, 0x0, 0x0,
    0x4, 0xf6, 0x0, 0x0, 0x0, 0x4, 0xd0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x4, 0x90,
    0x0, 0x0, 0x0, 0xd, 0xc0, 0x0, 0x0, 0x0,
    0x5f, 0x90, 0x0, 0x0, 0x0, 0xef, 0x10, 0x0,
    0x0, 0x5, 0xf7, 0x0, 0x0, 0x0, 0x5, 0xe0,
    0x0, 0x0, 0x0, 0x3, 0x40, 0x0, 0x0, 0x0,

    /* U+0038 "8" */
    0x9, 0xff, 0xe8, 0xff, 0xe2, 0x5, 0xff, 0xfe,
    0xaf, 0xff, 0xd0, 0xbf, 0x11, 0x0, 0x11, 0x8f,
    0x2b, 0xf0, 0x0, 0x0, 0x7, 0xf2, 0xbf, 0x0,
    0x0, 0x0, 0x7f, 0x2b, 0xf0, 0x0, 0x0, 0x7,
    0xf2, 0xbf, 0x0, 0x0, 0x0, 0x7f, 0x27, 0xff,
    0xfe, 0x8f, 0xff, 0xe0, 0x6f, 0xff, 0xea, 0xff,
    0xfc, 0xb, 0xf1, 0x11, 0x1, 0x18, 0xf2, 0xbf,
    0x0, 0x0, 0x0, 0x7f, 0x2b, 0xf0, 0x0, 0x0,
    0x7, 0xf2, 0xbf, 0x0, 0x0, 0x0, 0x7f, 0x2b,
    0xf0, 0x0, 0x0, 0x7, 0xf2, 0xbf, 0x11, 0x0,
    0x11, 0x8f, 0x25, 0xff, 0xfe, 0xaf, 0xff, 0xd0,
    0x9, 0xff, 0xe9, 0xff, 0xe2, 0x0,

    /* U+0039 "9" */
    0x9, 0xff, 0xe8, 0xff, 0xe2, 0x5, 0xff, 0xfe,
    0xaf, 0xff, 0xd0, 0xbf, 0x11, 0x0, 0x11, 0x8f,
    0x2b, 0xf0, 0x0, 0x0, 0x7, 0xf2, 0xbf, 0x0,
    0x0, 0x0, 0x7f, 0x2b, 0xf0, 0x0, 0x0, 0x7,
    0xf2, 0xbf, 0x0, 0x0, 0x0, 0x7f, 0x27, 0xff,
    0xfe, 0x8f, 0xff, 0xe0, 0xb, 0xff, 0xea, 0xff,
    0xfc, 0x0, 0x1, 0x11, 0x1, 0x18, 0xf2, 0x0,
    0x0, 0x0, 0x0, 0x7f, 0x20, 0x0, 0x0, 0x0,
    0x7, 0xf2, 0x0, 0x0, 0x0, 0x0, 0x7f, 0x20,
    0x0, 0x0, 0x0, 0x7, 0xf2, 0x0, 0x11, 0x0,
    0x11, 0x8f, 0x20, 0xaf, 0xfe, 0xaf, 0xff, 0xd0,
    0x9, 0xff, 0xe9, 0xff, 0xe2, 0x0,

    /* U+0043 "C" */
    0x9, 0xff, 0xd9, 0xff, 0xe2, 0x5f, 0xff, 0xea,
    0xff, 0xf2, 0xbf, 0x11, 0x0, 0x11, 0x0, 0xbf,
    0x0, 0x0, 0x0, 0x0, 0xbf, 0x0, 0x0, 0x0,
    0x0, 0xbf, 0x0, 0x0, 0x0, 0x0, 0xbf, 0x0,
    0x0, 0x0, 0x0, 0xae, 0x0, 0x0, 0x0, 0x0,
    0x38, 0x0, 0x0, 0x0, 0x0, 0xae, 0x0, 0x0,
    0x0, 0x0, 0xbf, 0x0, 0x0, 0x0, 0x0, 0xbf,
    0x0, 0x0, 0x0, 0x0, 0xbf, 0x0, 0x0, 0x0,
    0x0, 0xbf, 0x0, 0x0, 0x0, 0x0, 0xbf, 0x11,
    0x0, 0x11, 0x0, 0x5f, 0xff, 0xea, 0xff, 0xf2,
    0x9, 0xff, 0xd9, 0xff, 0xe1
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 184, .box_w = 11, .box_h = 17, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 94, .adv_w = 184, .box_w = 10, .box_h = 17, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 179, .adv_w = 184, .box_w = 11, .box_h = 17, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 273, .adv_w = 184, .box_w = 11, .box_h = 17, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 367, .adv_w = 184, .box_w = 11, .box_h = 16, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 455, .adv_w = 184, .box_w = 11, .box_h = 17, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 549, .adv_w = 184, .box_w = 11, .box_h = 17, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 643, .adv_w = 184, .box_w = 10, .box_h = 16, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 723, .adv_w = 184, .box_w = 11, .box_h = 17, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 817, .adv_w = 184, .box_w = 11, .box_h = 17, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 911, .adv_w = 184, .box_w = 10, .box_h = 17, .ofs_x = 0, .ofs_y = -3}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 48, .range_length = 10, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 67, .range_length = 1, .glyph_id_start = 11,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
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
    .cmap_num = 2,
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
const lv_font_t lv_font_digital_18 = {
#else
lv_font_t lv_font_digital_18 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 17,          /*The maximum line height required by the font*/
    .base_line = 3,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if LV_FONT_DIGITAL_18*/

