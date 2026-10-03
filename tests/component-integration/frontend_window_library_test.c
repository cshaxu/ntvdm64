#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/kvm-window/window_interface.h"
#include "lib/kvm-window/render.h"
#include "window_frame.h"
#include "text_frame.h"
#include "ntvwm-exe/text_frame.h"
#include "native_pc_font.h"

/* Test-local capture input, not a second production frame contract. */
typedef struct run16_native_frame_info {
    CONSOLE_SCREEN_BUFFER_INFOEX screen;
    CONSOLE_CURSOR_INFO cursor;
} run16_native_frame_info;

/* Test adapter only: production NTVWM packing -> common frontend decoder.
 * Keep the established pixel assertions, with no native renderer in NTCON. */
static DWORD frontend_window_native_frame_pointer(const run16_native_frame_info *info,
    const CHAR_INFO *cells,SIZE_T count,const POINT *pointer,kvm_window_frame *frame)
{
    console_text_style font={0};run16_console_video video={0};
    BYTE *payload=NULL;DWORD error;unsigned bank,glyph;
    frame->valid=0;font.font_height=14;
    for(bank=0;bank<2;++bank)for(glyph=0;glyph<256;++glyph)
        memcpy(font.fonts[bank][glyph],frontend_native_font[glyph],14);
    error=ntvwm_text_frame_pack(&info->screen,&info->cursor,cells,count,&font,
        &video.description,&payload);
    if(error)return error;
    video.pixels=payload;video.published_serial=1;
    error=frontend_window_decode_frame(&video,frame);
    HeapFree(GetProcessHeap(),0,payload);
    if(error)frame->valid=0;
    return error;
}
static DWORD frontend_window_native_frame(const run16_native_frame_info *info,
    const CHAR_INFO *cells,SIZE_T count,kvm_window_frame *frame)
{ return frontend_window_native_frame_pointer(info,cells,count,NULL,frame); }

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)

static lib_u32 *text_pixels(const kvm_window_frame *frame,lib_u32 *width,lib_u32 *height)
{
    lib_u32 *pixels;lib_bool valid=LIB_FALSE;kvm_window_rect changed;
    if(!kvm_window_frame_size(frame,width,height))return NULL;
    pixels=calloc((size_t)*width**height,sizeof(*pixels));
    if(!pixels)return NULL;
    if(!kvm_window_render_frame(frame,pixels,*width,*height,&valid,&changed)) {
        free(pixels);return NULL;
    }
    return pixels;
}

typedef struct input_count { unsigned hotkeys, keys; } input_count;

static lib_bool count_input(void *context, const kvm_input_event *event)
{
    input_count *count = context;
    if (event->type == KVM_EVENT_HOTKEY) ++count->hotkeys;
    if (event->type == KVM_EVENT_KEY) ++count->keys;
    return LIB_TRUE;
}

/* Exercise the real native converter, not only the VT parser. These checks
 * prove occupied-cell bounds and retained input, not font fallback quality.
 * CHAR_INFO cannot represent a base plus combining marks in one cell; S9's
 * terminal-cell adapter must retain that richer input instead of truncating it. */
static int native_unicode_bounds(kvm_window_frame *frame)
{
    static const WCHAR units[][2] = {{L'A', 0}, {0x4e2d, 0}, {0xd83d, 0xde00}, {0x2500, 0}};
    static const char *names[] = {"ASCII", "CJK replacement", "surrogate replacement", "PC line glyph"};
    /* Independent bytes from the pinned original V7VGA ROM: A, ?, and 196.
     * Assert exact pixels, not merely nonempty GDI output. */
    static const BYTE expected[][14]={
        {0,0,0x10,0x38,0x6c,0xc6,0xc6,0xfe,0xc6,0xc6,0xc6,0,0,0},
        {0,0,0x7c,0xc6,0xc6,0x0c,0x18,0x18,0,0x18,0x18,0,0,0},
        {0,0,0,0,0,0,0,0xff,0,0,0,0,0,0}};
    unsigned test;
    for (test = 0; test < 4; ++test) {
        run16_native_frame_info info = {0};
        CHAR_INFO cells[4], original[4];
        unsigned column, x, y, ink = 0, span = (test==1 || test==2) ? 2 : 1;
        lib_u32 width,height,*pixels;
        info.screen.dwSize.X = 4; info.screen.dwSize.Y = 1;
        info.screen.srWindow.Right = 3;
        info.screen.ColorTable[7] = RGB(255, 255, 255);
        info.cursor.dwSize = 25;
        for (column = 0; column < 4; ++column) {
            cells[column].Char.UnicodeChar = L' ';
            cells[column].Attributes = 7;
        }
        cells[1].Char.UnicodeChar = units[test][0];
        if (test == 1) {
            cells[1].Attributes |= COMMON_LVB_LEADING_BYTE;
            cells[2].Attributes |= COMMON_LVB_TRAILING_BYTE;
        } else if (test == 2) cells[2].Char.UnicodeChar = units[test][1];
        memcpy(original, cells, sizeof(cells));
        CHECK(frontend_window_native_frame(&info, cells, 4, frame) == ERROR_SUCCESS);
        CHECK(!memcmp(cells, original, sizeof(cells)));
        CHECK(frame->valid && !frame->graphics);
        pixels=text_pixels(frame,&width,&height);CHECK(pixels && width==32 && height==14);
        for (y = 0; y < height; ++y) for (x = 0; x < 32; ++x) {
            DWORD rgb = pixels[y * width + x];
            unsigned glyph=test==0 ? 0 : test==3 ? 2 : 1;
            BOOL set=x>=8 && x<16 && (expected[glyph][y]&(0x80>>(x-8)));
            CHECK(rgb==(set ? 0xffffffu : 0));
            CHECK(rgb == 0 || rgb == 0xffffff);
            if (x >= 8 && x < (1 + span) * 8) ink += rgb != 0;
            else CHECK(rgb == 0);
        }
        free(pixels);
        CHECK(ink != 0);
        printf("PASS native Unicode carrier: %s has ink within %u cells; source unchanged\n",
            names[test], span);
    }
    puts("PASS exact original V7VGA bitmap pixels, PC line mapping and explicit '?' fallback; wide trailing cell blank, Unicode input unchanged");
    return 0;
}

static int native_style_colors(kvm_window_frame *frame)
{
    run16_native_frame_info info={0};CHAR_INFO cells[4]={0};unsigned i;
    lib_u32 width,height,*pixels;
    info.screen.dwSize=(COORD){4,1};info.screen.srWindow.Right=3;
    info.screen.ColorTable[7]=RGB(10,20,30);info.screen.ColorTable[15]=RGB(40,50,60);
    info.cursor.dwSize=25;
    for(i=0;i<4;++i) {
        cells[i].Char.UnicodeChar=L' ';
        cells[i].Attributes=(i<2 ? 7 : 15)|((i&1) ? COMMON_LVB_UNDERSCORE : 0);
    }
    CHECK(!frontend_window_native_frame(&info,cells,4,frame));
    CHECK(!frame->graphics);
    pixels=text_pixels(frame,&width,&height);CHECK(pixels && width==32 && height==14);
    for(i=0;i<4;++i) {
        DWORD rgb=pixels[13*width+i*8];
        CHECK(rgb==(!(i&1) ? 0 : i<2 ? 0x0a141e : 0x28323c));
    }
    free(pixels);
    puts("PASS shared style byte: underline is independent of foreground intensity");
    return 0;
}

int main(void)
{
    kvm_window_frame *frame = calloc(1, sizeof(*frame));
    lib_u32 *pixels = calloc(1280u * 3072u, sizeof(*pixels));
    lib_u32 width, height;
    lib_bool valid = LIB_FALSE;
    kvm_window_rect changed = {0};
    kvm_hotkey_registry registry;
    kvm_hotkey_matcher matcher;
    kvm_input_event key = {0};
    input_count count = {0};
    kvm_window *window = NULL;
    kvm_window_options options = {0};
    {
        kvm_window_rect work={0,0,800,600};lib_i32 fitted_width,fitted_height;
        CHECK(kvm_window_fit_client_size(&work,16,40,640,400,&fitted_width,&fitted_height));
        CHECK(fitted_width==640 && fitted_height==400);
        CHECK(kvm_window_fit_client_size(&work,16,40,1280,768,&fitted_width,&fitted_height));
        CHECK(fitted_width==784 && fitted_height==471);
        puts("PASS library Window sizing: frame-sized by default; monitor fit only when necessary");
    }
    run16_console_video video = {0};
    console_video_description description = {9, 2, 4, 1, 8, {0}};
    BYTE dib[8] = {0x80, 0x80, 0xee, 0xee, 0x40, 0, 0xee, 0xee};
    run16_native_frame_info native = {0};
    CHAR_INFO *native_cells;
    CHECK(frame && pixels);
    CHECK(native_unicode_bounds(frame) == 0);
    CHECK(native_style_colors(frame) == 0);
    frame->valid=frame->graphics=LIB_TRUE;
    frame->image.width=frame->image.stride=1600;frame->image.height=350;
    CHECK(kvm_window_frame_validate(frame)==LIB_STATUS_UNSUPPORTED);
    {
        kvm_window_rect work={0,0,1920,1080};lib_i32 fitted_width,fitted_height;
        CHECK(kvm_window_fit_client_size(&work,16,40,1600,350,&fitted_width,&fitted_height));
        CHECK(fitted_width==1600 && fitted_height==350);
    }
    puts("CONFIRMED LIMIT: 1600x350 fits monitor but imported frame transport rejects it; not feature acceptance");
    memset(frame,0,sizeof(*frame));
    frame->valid = LIB_TRUE;
    frame->text.base.text_columns = 80;
    frame->text.base.text_rows = 50;
    frame->text.base.font_height = 8;
    frame->text.base.text_palette[1] = 0x123456;
    frame->text.base.cells[49 * KVM_TEXT_COLUMNS + 79].glyph_index = 65;
    frame->text.base.cells[49 * KVM_TEXT_COLUMNS + 79].glyph_bank = 1;
    frame->text.base.cells[49 * KVM_TEXT_COLUMNS + 79].foreground = 1;
    frame->text.secondary_font[65 * KVM_WINDOW_FONT_HEIGHT + 7] = 1;
    CHECK(kvm_window_frame_validate(frame) == LIB_STATUS_OK);
    CHECK(kvm_window_frame_size(frame, &width, &height) && width == 640 && height == 400);
    CHECK(kvm_window_render_frame(frame, pixels, width, height, &valid, &changed));
    CHECK(pixels[399 * 640 + 639] == 0x123456);
    CHECK(!kvm_window_render_frame(frame, pixels, width, height, &valid, &changed));
    frame->text.base.text_rows = 43;
    CHECK(kvm_window_frame_size(frame, &width, &height) && height == 344);
    frame->text.base.text_rows = 97;
    CHECK(kvm_window_frame_validate(frame) == LIB_STATUS_UNSUPPORTED);
    memset(frame, 0, sizeof(*frame));
    frame->valid = frame->graphics = LIB_TRUE;
    frame->image.width = frame->image.stride = 2;
    frame->image.height = 1;
    frame->image.palette[7] = 0xff00aa;
    frame->image.pixels[1] = 7;
    valid = LIB_FALSE;
    CHECK(kvm_window_render_frame(frame, pixels, 2, 1, &valid, &changed));
    CHECK(pixels[0] == 0 && pixels[1] == 0xff00aa);
    frame->image.stride = 1;
    CHECK(kvm_window_frame_validate(frame) == LIB_STATUS_INVALID_ARGUMENT);
    kvm_hotkey_registry_initialize(&registry);
    CHECK(kvm_hotkey_registry_register(&registry, 'F', KVM_KEY_MODIFIER_CONTROL |
        KVM_KEY_MODIFIER_ALT, "display") == LIB_STATUS_OK);
    kvm_hotkey_matcher_initialize(&matcher, &registry);
    key.type = KVM_EVENT_KEY;
    key.data.key.key = 'F'; key.data.key.scan_code = 0x21;
    key.data.key.modifiers = KVM_KEY_MODIFIER_CONTROL | KVM_KEY_MODIFIER_ALT;
    key.data.key.pressed = LIB_TRUE;
    CHECK(kvm_hotkey_matcher_submit(&matcher, &key, count_input, &count, LIB_TRUE));
    CHECK(count.hotkeys == 1 && count.keys == 0);
    key.data.key.pressed = LIB_FALSE;
    CHECK(kvm_hotkey_matcher_submit(&matcher, &key, count_input, &count, LIB_TRUE));
    CHECK(count.hotkeys == 1 && count.keys == 0);
    kvm_hotkey_matcher_discard(&matcher);
    /* Link the real Win32 leaf, but never create a Window on the owner's desktop. */
    CHECK(kvm_window_create(&window, &options) == LIB_STATUS_INVALID_ARGUMENT);
    CHECK(window == NULL);
    description.palette[1] = 0x2468ac;
    CHECK(run16_console_video_begin(&video, 1, &description) == ERROR_SUCCESS);
    CHECK(run16_console_video_data(&video, 1, 0, dib, 4) == ERROR_SUCCESS);
    CHECK(frontend_window_decode_frame(&video, frame) == ERROR_NO_DATA && !frame->valid);
    CHECK(run16_console_video_data(&video, 1, 4, dib + 4, 4) == ERROR_SUCCESS);
    CHECK(frontend_window_decode_frame(&video, frame) == ERROR_SUCCESS);
    CHECK(frame->image.width == 9 && frame->image.stride == 9 && frame->image.height == 2);
    CHECK(frame->image.pixels[0] == 1 && frame->image.pixels[8] == 1);
    CHECK(frame->image.pixels[9] == 0 && frame->image.pixels[10] == 1);
    CHECK(frame->image.palette[1] == 0x2468ac);
    CHECK(run16_console_video_begin(&video, 2, &description) == ERROR_SUCCESS);
    CHECK(frontend_window_decode_frame(&video, frame) == ERROR_SUCCESS); /* Retain complete frame. */
    CHECK(run16_console_video_text(&video, 3) == ERROR_SUCCESS);
    CHECK(frontend_window_decode_frame(&video, frame) == ERROR_NO_DATA && !frame->valid);
    description.width = 3; description.depth = 8;
    CHECK(run16_console_video_begin(&video, 4, &description) == ERROR_SUCCESS);
    CHECK(run16_console_video_data(&video, 4, 0, dib, 8) == ERROR_SUCCESS);
    CHECK(frontend_window_decode_frame(&video, frame) == ERROR_SUCCESS);
    CHECK(frame->image.pixels[2] == 0xee && frame->image.pixels[3] == 0x40);
    run16_console_video_dispose(&video);
    {
        console_video_description text={80,50,160,0,0,{0},CONSOLE_VIDEO_TEXT_FRAME};
        console_text_style *style;BYTE *payload;
        text.bytes=sizeof(console_text_style)+text.stride*text.height;
        payload=calloc(1,text.bytes);CHECK(payload);style=(console_text_style *)payload;
        style->font_height=8;style->attribute_font_select=1;
        style->fonts[1][65][7]=1;text.palette[9]=0x123456;
        payload[sizeof(*style)+(49*80+79)*2]=65;
        payload[sizeof(*style)+(49*80+79)*2+1]=9;
        CHECK(!run16_console_video_begin(&video,1,&text));
        CHECK(!run16_console_video_data(&video,1,0,payload,16384));
        CHECK(frontend_window_decode_frame(&video,frame)==ERROR_NO_DATA);
        CHECK(!run16_console_video_data(&video,1,16384,payload+16384,text.bytes-16384));
        CHECK(!frontend_window_decode_frame(&video,frame) && !frame->graphics);
        CHECK(frame->text.base.text_rows==50 && frame->text.base.font_height==8);
        valid=LIB_FALSE;
        CHECK(kvm_window_render_frame(frame,pixels,640,400,&valid,&changed));
        CHECK(pixels[399*640+639]==0x123456);
        CHECK(!run16_console_video_begin(&video,2,&text));
        style->font_height=33;
        CHECK(run16_console_video_data(&video,2,0,payload,text.bytes)==ERROR_INVALID_DATA);
        CHECK(video.published_serial==1 && !frontend_window_decode_frame(&video,frame));
        style->font_height=8;style->cursor_start=INT32_MAX;
        CHECK(!run16_console_video_begin(&video,3,&text));
        CHECK(run16_console_video_data(&video,3,0,payload,text.bytes)==ERROR_INVALID_DATA);
        CHECK(video.published_serial==1);
        style->cursor_start=0;text.height=25;text.bytes=sizeof(*style)+160*25;
        style->font_height=20;style->fonts[0][65][19]=1;style->attribute_font_select=0;
        payload[sizeof(*style)]=65;payload[sizeof(*style)+1]=9;
        CHECK(!run16_console_video_begin(&video,4,&text));
        CHECK(!run16_console_video_data(&video,4,0,payload,text.bytes));
        CHECK(!frontend_window_decode_frame(&video,frame) && !frame->graphics);
        valid=LIB_FALSE;
        CHECK(kvm_window_render_frame(frame,pixels,640,500,&valid,&changed));
        CHECK(pixels[19*640+7]==0x123456);
        --text.bytes;
        CHECK(run16_console_video_begin(&video,5,&text)==ERROR_INVALID_DATA);
        /* Text transport is not subject to the 768-line DIB limit. The
         * unchanged library directly renders 80x50 with a 16-line font. */
        text.height=50;text.bytes=sizeof(*style)+160*50;
        style->font_height=16;style->attribute_font_select=1;
        style->fonts[1][65][15]=1;
        CHECK(!run16_console_video_begin(&video,6,&text));
        CHECK(!run16_console_video_data(&video,6,0,payload,text.bytes));
        CHECK(!frontend_window_decode_frame(&video,frame) && !frame->graphics);
        CHECK(frame->text.base.text_rows==50 && frame->text.base.font_height==16);
        valid=LIB_FALSE;
        CHECK(kvm_window_render_frame(frame,pixels,640,800,&valid,&changed));
        CHECK(pixels[799*640+639]==0x123456);
        free(payload);run16_console_video_dispose(&video);
        /* The normal native 16-line font plus a per-cell underline must not
         * turn a supported 80x50 TEXT page into an oversized DIB. */
        text.stride=240;text.bytes=sizeof(*style)+text.stride*50;
        payload=calloc(1,text.bytes);CHECK(payload);style=(console_text_style *)payload;
        style->font_height=16;text.palette[7]=0xabcdef;
        for(unsigned cell=0;cell<4000;++cell) {
            payload[sizeof(*style)+cell*3]=' ';
            payload[sizeof(*style)+cell*3+1]=7;
        }
        payload[sizeof(*style)+3999*3+2]=CONSOLE_TEXT_UNDERLINE;
        CHECK(!run16_console_video_begin(&video,1,&text));
        CHECK(!run16_console_video_data(&video,1,0,payload,text.bytes));
        CHECK(!frontend_window_decode_frame(&video,frame) && !frame->graphics);
        valid=LIB_FALSE;
        CHECK(kvm_window_render_frame(frame,pixels,640,800,&valid,&changed));
        CHECK(pixels[799*640+639]==0xabcdef && pixels[799*640+631]==0);
        CHECK(pixels[798*640+639]==0);
        CHECK(payload[sizeof(*style)+3999*3]==' ' && style->fonts[0][' '][15]==0);
        style->attribute_font_select=1;
        for(unsigned glyph=0;glyph<256;++glyph)for(unsigned line=0;line<16;++line) {
            style->fonts[0][glyph][line]=(BYTE)(glyph+line);
            style->fonts[1][glyph][line]=(BYTE)(glyph+line+1);
        }
        for(unsigned cell=0;cell<4000;++cell) {
            payload[sizeof(*style)+cell*3]=(BYTE)(cell%256);
            payload[sizeof(*style)+cell*3+1]=7;
            payload[sizeof(*style)+cell*3+2]=cell>=256 && cell<512 ? CONSOLE_TEXT_UNDERLINE : 0;
        }
        CHECK(!run16_console_video_begin(&video,2,&text));
        CHECK(!run16_console_video_data(&video,2,0,payload,text.bytes));
        CHECK(!frontend_window_decode_frame(&video,frame) && !frame->graphics);
        for(unsigned cell=0;cell<512;++cell) {
            kvm_text_cell mapped=frame->text.base.cells[(cell/80)*KVM_TEXT_COLUMNS+cell%80];
            const lib_u8 *font=mapped.glyph_bank ? frame->text.secondary_font : frame->text.font;
            CHECK(mapped.foreground==7 && mapped.background==0);
            for(unsigned line=0;line<16;++line)
                CHECK(font[mapped.glyph_index*KVM_WINDOW_FONT_HEIGHT+line]==
                    (BYTE)(cell%256+line));
            CHECK(frame->text.styles[(cell/80)*KVM_TEXT_COLUMNS+cell%80]==
                (cell>=256 ? CONSOLE_TEXT_UNDERLINE : 0));
        }
        /* Styles are stored per cell, so a 513th distinct glyph/style
         * combination no longer exhausts the two source font banks. */
        payload[sizeof(*style)+512*3+1]=15;
        CHECK(!run16_console_video_begin(&video,3,&text));
        CHECK(!run16_console_video_data(&video,3,0,payload,text.bytes));
        CHECK(!frontend_window_decode_frame(&video,frame) && !frame->graphics);
        CHECK(style->fonts[0][0][15]==15 && style->fonts[1][0][15]==16);
        free(payload);run16_console_video_dispose(&video);
        puts("PASS copied DOS text: complete-frame publication, 80x50 dual font, tall glyph text and malformed frame retention");
    }
    native_cells = calloc(5001u * 300u, sizeof(*native_cells));
    CHECK(native_cells != NULL);
    native.screen.dwSize.X = 5001; native.screen.dwSize.Y = 300;
    native.screen.srWindow.Left = native.screen.srWindow.Right = 5000;
    native.screen.srWindow.Top = native.screen.srWindow.Bottom = 299;
    native.screen.ColorTable[2] = RGB(5, 10, 15);
    native.screen.ColorTable[3] = RGB(21, 22, 23);
    native.cursor.dwSize = 25;
    native_cells[5001 * 299 + 5000].Char.UnicodeChar = L' ';
    native_cells[5001 * 299 + 5000].Attributes = 0x23 | COMMON_LVB_UNDERSCORE;
    CHECK(frontend_window_native_frame(&native, native_cells, 5001u * 300u, frame) == ERROR_SUCCESS);
    CHECK(!frame->graphics && frame->text.base.text_columns==1 && frame->text.base.text_rows==1);
    valid=LIB_FALSE;
    CHECK(kvm_window_render_frame(frame,pixels,8,14,&valid,&changed));
    CHECK(pixels[0] == 0x050a0f && pixels[13*8] == 0x151617);
    native.screen.dwCursorPosition.X = 5000; native.screen.dwCursorPosition.Y = 299;
    native.cursor.bVisible = TRUE;
    CHECK(frontend_window_native_frame(&native, native_cells, 5001u * 300u, frame) == ERROR_SUCCESS);
    CHECK(!frame->graphics && frame->text.base.cursor_visible);
    CHECK(frame->text.base.cursor_top==10 && frame->text.base.cursor_bottom==13);
    CHECK(frontend_window_native_frame(&native, native_cells, 1, frame) == ERROR_INVALID_DATA && !frame->valid);
    /* Large backing buffers and large visible viewports are distinct. */
    native.cursor.bVisible = FALSE;
    native.screen.srWindow.Left = native.screen.srWindow.Top = 0;
    native.screen.srWindow.Right = 159;
    native.screen.srWindow.Bottom = 53;
    native_cells[5001 * 53 + 159].Char.UnicodeChar = L' ';
    native_cells[5001 * 53 + 159].Attributes = 0x23 | COMMON_LVB_UNDERSCORE;
    CHECK(frontend_window_native_frame(&native, native_cells, 5001u * 300u, frame) == ERROR_SUCCESS);
    CHECK(frame->valid && !frame->graphics && frame->text.base.text_columns==160 &&
        frame->text.base.text_rows==54);
    valid=LIB_FALSE;
    CHECK(kvm_window_render_frame(frame,pixels,1280,756,&valid,&changed));
    CHECK(pixels[755 * 1280 + 1279] == 0x151617);
    native.screen.srWindow.Right=119;native.screen.srWindow.Bottom=29;
    native.screen.dwCursorPosition=(COORD){4,2};native.cursor.bVisible=TRUE;
    CHECK(frontend_window_native_frame(&native,native_cells,5001u*300u,frame)==ERROR_SUCCESS);
    CHECK(!frame->graphics && frame->text.base.text_columns==120 &&
        frame->text.base.text_rows==30 && frame->text.base.cursor_visible);
    {
        kvm_window_rect display={0,0,960,420},cursor_rect;
        CHECK(kvm_window_cursor_rect(frame,&display,&cursor_rect));
        CHECK(cursor_rect.left==32 && cursor_rect.right==40 &&
            cursor_rect.bottom>cursor_rect.top);
    }
    native.screen.srWindow.Right=159;native.screen.srWindow.Bottom=95;
    native.cursor.bVisible=FALSE;
    CHECK(frontend_window_native_frame(&native,native_cells,5001u*300u,frame)==ERROR_SUCCESS);
    CHECK(!frame->graphics && frame->text.base.text_columns==160 &&
        frame->text.base.text_rows==96);
    native.screen.srWindow.Right = 160;
    CHECK(frontend_window_native_frame(&native, native_cells, 5001u * 300u, frame) == ERROR_NOT_SUPPORTED && !frame->valid);
    native.screen.srWindow.Right = 159;
    native.screen.srWindow.Bottom = 96;
    CHECK(frontend_window_native_frame(&native, native_cells, 5001u * 300u, frame) == ERROR_NOT_SUPPORTED && !frame->valid);
    {
        console_video_description text={120,30,360,0,0,{0},CONSOLE_VIDEO_TEXT_FRAME};
        console_text_style *style;BYTE *payload;
        kvm_window_rect display={0,0,960,960},cursor_a,cursor_b;
        text.bytes=sizeof(console_text_style)+text.stride*text.height;
        payload=calloc(1,text.bytes);CHECK(payload);
        style=(console_text_style *)payload;style->font_height=32;
        style->cursor_column=1;style->cursor_row=1;style->cursor_visible=TRUE;
        style->cursor_start=30;style->cursor_height=2;
        style->cursor_start1=0;style->cursor_height1=2;
        style->fonts[0][65][31]=0x80;
        payload[sizeof(*style)+(120+1)*3]=65;
        payload[sizeof(*style)+(120+1)*3+1]=7;
        payload[sizeof(*style)+(120+1)*3+2]=CONSOLE_TEXT_UNDERLINE;
        text.palette[7]=0xffffff;
        CHECK(!run16_console_video_begin(&video,11,&text));
        CHECK(!run16_console_video_data(&video,11,0,payload,text.bytes));
        CHECK(!frontend_window_decode_frame(&video,frame) && !frame->graphics);
        CHECK(kvm_window_frame_size(frame,&width,&height) && width==960 && height==960);
        CHECK(kvm_window_cursor_rect(frame,&display,&cursor_a));
        CHECK(kvm_window_secondary_cursor_rect(frame,&display,&cursor_b));
        CHECK(cursor_a.top==62 && cursor_a.bottom==64 &&
            cursor_b.top==32 && cursor_b.bottom==34);
        valid=LIB_FALSE;
        CHECK(kvm_window_render_frame(frame,pixels,width,height,&valid,&changed));
        CHECK(pixels[63*width+8]==0xffffff && pixels[32*width+8]==0);
        free(payload);run16_console_video_dispose(&video);
    }
    puts("PASS native 160-column text; oversized protocol view rejected without font shrinking");
    free(native_cells);
    free(pixels); free(frame);
    puts("PASS nxvm x86 closure: 80x50/43, font banks, indexed graphics, CAF make/break, invalid lifecycle");
    puts("PASS frontend frames: partial/complete/TEXT, monochrome/top-down/padding, native viewport/scrollback/underline/cursor");
    return 0;
}
