/*******************************************************************************
 * Size: 16 px
 * Bpp: 4
 * Opts: --bpp 4 --size 16 --no-compress --font digitaldreamfatnarrow.ttf --symbols 0123456789C --format lvgl -o lv_font_digital_16.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef LV_FONT_DIGITAL_16
#define LV_FONT_DIGITAL_16 1
#endif

#if LV_FONT_DIGITAL_16

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0030 "0" */
    0xc, 0xff, 0x9f, 0xff, 0x20, 0x8f, 0xff, 0x8e,
    0xff, 0xd0, 0xbc, 0x0, 0x0, 0x4f, 0xf0, 0xbc,
    0x0, 0x0, 0xcf, 0xf0, 0xbc, 0x0, 0x5, 0xfd,
    0xf0, 0xbc, 0x0, 0xc, 0xe7, 0xf0, 0xbc, 0x0,
    0xe, 0x67, 0xf0, 0x78, 0x0, 0x5, 0x3, 0xc0,
    0x78, 0x4, 0x10, 0x3, 0xc0, 0xbc, 0x1f, 0x30,
    0x7, 0xf0, 0xbc, 0x9f, 0x20, 0x7, 0xf0, 0xbd,
    0xfa, 0x0, 0x7, 0xf0, 0xbf, 0xf2, 0x0, 0x7,
    0xf0, 0xbf, 0x90, 0x0, 0x7, 0xf0, 0x8f, 0xff,
    0x8e, 0xff, 0xd0, 0xc, 0xff, 0x9f, 0xff, 0x20,

    /* U+0031 "1" */
    0xb, 0xff, 0x70, 0x0, 0x0, 0xbf, 0xff, 0x30,
    0x0, 0x0, 0x1, 0xf6, 0x0, 0x0, 0x0, 0x1f,
    0x60, 0x0, 0x0, 0x1, 0xf6, 0x0, 0x0, 0x0,
    0x1f, 0x60, 0x0, 0x0, 0x1, 0xf6, 0x0, 0x0,
    0x0, 0xc, 0x30, 0x0, 0x0, 0x0, 0xc2, 0x0,
    0x0, 0x0, 0x1f, 0x60, 0x0, 0x0, 0x1, 0xf6,
    0x0, 0x0, 0x0, 0x1f, 0x60, 0x0, 0x0, 0x1,
    0xf6, 0x0, 0x0, 0x0, 0x1f, 0x60, 0x0, 0xa,
    0xff, 0xff, 0xfd, 0x10, 0xcf, 0xf9, 0xff, 0xe2,

    /* U+0032 "2" */
    0xc, 0xff, 0x9f, 0xff, 0x20, 0xb, 0xff, 0x8e,
    0xff, 0xd0, 0x0, 0x0, 0x0, 0x7, 0xf0, 0x0,
    0x0, 0x0, 0x7, 0xf0, 0x0, 0x0, 0x0, 0x7,
    0xf0, 0x0, 0x0, 0x0, 0x7, 0xf0, 0x0, 0x0,
    0x0, 0x7, 0xf0, 0xb, 0xff, 0x8e, 0xff, 0xd0,
    0x7f, 0xff, 0x9f, 0xff, 0x20, 0xbc, 0x0, 0x0,
    0x0, 0x0, 0xbc, 0x0, 0x0, 0x0, 0x0, 0xbc,
    0x0, 0x0, 0x0, 0x0, 0xbc, 0x0, 0x0, 0x0,
    0x0, 0xbc, 0x0, 0x0, 0x0, 0x0, 0x8f, 0xff,
    0x8e, 0xfe, 0x20, 0xc, 0xff, 0x9f, 0xff, 0x20,

    /* U+0033 "3" */
    0xb, 0xff, 0x9f, 0xff, 0x20, 0xb, 0xff, 0x8e,
    0xff, 0xd0, 0x0, 0x0, 0x0, 0x7, 0xf0, 0x0,
    0x0, 0x0, 0x7, 0xf0, 0x0, 0x0, 0x0, 0x7,
    0xf0, 0x0, 0x0, 0x0, 0x7, 0xf0, 0x0, 0x0,
    0x0, 0x7, 0xf0, 0x0, 0x0, 0x2e, 0xff, 0xd0,
    0x0, 0x0, 0x2f, 0xff, 0xc0, 0x0, 0x0, 0x0,
    0x7, 0xf0, 0x0, 0x0, 0x0, 0x7, 0xf0, 0x0,
    0x0, 0x0, 0x7, 0xf0, 0x0, 0x0, 0x0, 0x7,
    0xf0, 0x0, 0x0, 0x0, 0x7, 0xf0, 0xa, 0xff,
    0x8e, 0xff, 0xd0, 0xc, 0xff, 0x9f, 0xff, 0x20,

    /* U+0034 "4" */
    0x0, 0x0, 0x0, 0x0, 0x0, 0x77, 0x0, 0x0,
    0x3, 0xc0, 0xbc, 0x0, 0x0, 0x7, 0xf0, 0xbc,
    0x0, 0x0, 0x7, 0xf0, 0xbc, 0x0, 0x0, 0x7,
    0xf0, 0xbc, 0x0, 0x0, 0x7, 0xf0, 0xbc, 0x0,
    0x0, 0x7, 0xf0, 0x8f, 0xee, 0x8e, 0xef, 0xd0,
    0xc, 0xff, 0x9f, 0xff, 0xc0, 0x0, 0x0, 0x0,
    0x7, 0xf0, 0x0, 0x0, 0x0, 0x7, 0xf0, 0x0,
    0x0, 0x0, 0x7, 0xf0, 0x0, 0x0, 0x0, 0x7,
    0xf0, 0x0, 0x0, 0x0, 0x7, 0xf0, 0x0, 0x0,
    0x0, 0x3, 0xc0, 0x0, 0x0, 0x0, 0x0, 0x0,

    /* U+0035 "5" */
    0xc, 0xff, 0x9f, 0xff, 0x20, 0x8f, 0xff, 0x8e,
    0xfe, 0x20, 0xbc, 0x0, 0x0, 0x0, 0x0, 0xbc,
    0x0, 0x0, 0x0, 0x0, 0xbc, 0x0, 0x0, 0x0,
    0x0, 0xbc, 0x0, 0x0, 0x0, 0x0, 0xbc, 0x0,
    0x0, 0x0, 0x0, 0x8f, 0xff, 0x8e, 0xfe, 0x20,
    0xc, 0xff, 0x9f, 0xff, 0xc0, 0x0, 0x0, 0x0,
    0x7, 0xf0, 0x0, 0x0, 0x0, 0x7, 0xf0, 0x0,
    0x0, 0x0, 0x7, 0xf0, 0x0, 0x0, 0x0, 0x7,
    0xf0, 0x0, 0x0, 0x0, 0x7, 0xf0, 0xb, 0xff,
    0x8e, 0xff, 0xd0, 0xc, 0xff, 0x9f, 0xff, 0x20,

    /* U+0036 "6" */
    0xc, 0xff, 0x9f, 0xff, 0x20, 0x8f, 0xff, 0x8e,
    0xfe, 0x20, 0xbc, 0x0, 0x0, 0x0, 0x0, 0xbc,
    0x0, 0x0, 0x0, 0x0, 0xbc, 0x0, 0x0, 0x0,
    0x0, 0xbc, 0x0, 0x0, 0x0, 0x0, 0xbc, 0x0,
    0x0, 0x0, 0x0, 0x8f, 0xff, 0x8e, 0xfe, 0x20,
    0x7f, 0xff, 0x9f, 0xff, 0xc0, 0xbc, 0x0, 0x0,
    0x7, 0xf0, 0xbc, 0x0, 0x0, 0x7, 0xf0, 0xbc,
    0x0, 0x0, 0x7, 0xf0, 0xbc, 0x0, 0x0, 0x7,
    0xf0, 0xbc, 0x0, 0x0, 0x7, 0xf0, 0x8f, 0xff,
    0x8e, 0xff, 0xd0, 0xc, 0xff, 0x9f, 0xff, 0x20,

    /* U+0037 "7" */
    0xb, 0xff, 0x9f, 0xfe, 0x20, 0xbf, 0xf8, 0xef,
    0xf1, 0x0, 0x0, 0x0, 0x4d, 0x0, 0x0, 0x0,
    0xc, 0xd0, 0x0, 0x0, 0x5, 0xf6, 0x0, 0x0,
    0x0, 0xcd, 0x0, 0x0, 0x0, 0xe, 0x50, 0x0,
    0x0, 0x0, 0x50, 0x0, 0x0, 0x4, 0x10, 0x0,
    0x0, 0x1, 0xf3, 0x0, 0x0, 0x0, 0x9f, 0x20,
    0x0, 0x0, 0x2f, 0xa0, 0x0, 0x0, 0x8, 0xf2,
    0x0, 0x0, 0x0, 0x89, 0x0, 0x0, 0x0, 0x3,
    0x10, 0x0, 0x0, 0x0,

    /* U+0038 "8" */
    0xc, 0xff, 0x9f, 0xff, 0x20, 0x8f, 0xff, 0x8e,
    0xff, 0xd0, 0xbc, 0x0, 0x0, 0x7, 0xf0, 0xbc,
    0x0, 0x0, 0x7, 0xf0, 0xbc, 0x0, 0x0, 0x7,
    0xf0, 0xbc, 0x0, 0x0, 0x7, 0xf0, 0xbc, 0x0,
    0x0, 0x7, 0xf0, 0x8f, 0xff, 0x8e, 0xff, 0xd0,
    0x7f, 0xff, 0x9f, 0xff, 0xc0, 0xbc, 0x0, 0x0,
    0x7, 0xf0, 0xbc, 0x0, 0x0, 0x7, 0xf0, 0xbc,
    0x0, 0x0, 0x7, 0xf0, 0xbc, 0x0, 0x0, 0x7,
    0xf0, 0xbc, 0x0, 0x0, 0x7, 0xf0, 0x8f, 0xff,
    0x8e, 0xff, 0xd0, 0xc, 0xff, 0x9f, 0xff, 0x20,

    /* U+0039 "9" */
    0xc, 0xff, 0x9f, 0xff, 0x20, 0x8f, 0xff, 0x8e,
    0xff, 0xd0, 0xbc, 0x0, 0x0, 0x7, 0xf0, 0xbc,
    0x0, 0x0, 0x7, 0xf0, 0xbc, 0x0, 0x0, 0x7,
    0xf0, 0xbc, 0x0, 0x0, 0x7, 0xf0, 0xbc, 0x0,
    0x0, 0x7, 0xf0, 0x8f, 0xff, 0x8e, 0xff, 0xd0,
    0xc, 0xff, 0x9f, 0xff, 0xc0, 0x0, 0x0, 0x0,
    0x7, 0xf0, 0x0, 0x0, 0x0, 0x7, 0xf0, 0x0,
    0x0, 0x0, 0x7, 0xf0, 0x0, 0x0, 0x0, 0x7,
    0xf0, 0x0, 0x0, 0x0, 0x7, 0xf0, 0xb, 0xff,
    0x8e, 0xff, 0xd0, 0xc, 0xff, 0x9f, 0xff, 0x20,

    /* U+0043 "C" */
    0xc, 0xff, 0x9f, 0xfe, 0x28, 0xff, 0xf8, 0xef,
    0xd1, 0xbc, 0x0, 0x0, 0x0, 0xb, 0xc0, 0x0,
    0x0, 0x0, 0xbc, 0x0, 0x0, 0x0, 0xb, 0xc0,
    0x0, 0x0, 0x0, 0xbc, 0x0, 0x0, 0x0, 0x8,
    0x80, 0x0, 0x0, 0x0, 0x87, 0x0, 0x0, 0x0,
    0xb, 0xc0, 0x0, 0x0, 0x0, 0xbc, 0x0, 0x0,
    0x0, 0xb, 0xc0, 0x0, 0x0, 0x0, 0xbc, 0x0,
    0x0, 0x0, 0xb, 0xc0, 0x0, 0x0, 0x0, 0x8f,
    0xff, 0x8e, 0xfd, 0x10, 0xcf, 0xf9, 0xff, 0xe2
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 164, .box_w = 10, .box_h = 16, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 80, .adv_w = 164, .box_w = 9, .box_h = 16, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 152, .adv_w = 164, .box_w = 10, .box_h = 16, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 232, .adv_w = 164, .box_w = 10, .box_h = 16, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 312, .adv_w = 164, .box_w = 10, .box_h = 16, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 392, .adv_w = 164, .box_w = 10, .box_h = 16, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 472, .adv_w = 164, .box_w = 10, .box_h = 16, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 552, .adv_w = 164, .box_w = 9, .box_h = 15, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 620, .adv_w = 164, .box_w = 10, .box_h = 16, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 700, .adv_w = 164, .box_w = 10, .box_h = 16, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 780, .adv_w = 164, .box_w = 9, .box_h = 16, .ofs_x = 0, .ofs_y = -3}
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
const lv_font_t lv_font_digital_16 = {
#else
lv_font_t lv_font_digital_16 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 16,          /*The maximum line height required by the font*/
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



#endif /*#if LV_FONT_DIGITAL_16*/

