#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/kvm-window/window_interface.h"
#include "lib/kvm-window/render.h"
#include "window_frame.h"

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)

typedef struct input_count { unsigned hotkeys, keys; } input_count;

static lib_bool count_input(void *context, const kvm_input_event *event)
{
    input_count *count = context;
    if (event->type == KVM_EVENT_HOTKEY) ++count->hotkeys;
    if (event->type == KVM_EVENT_KEY) ++count->keys;
    return LIB_TRUE;
}

int main(void)
{
    kvm_window_frame *frame = calloc(1, sizeof(*frame));
    lib_u32 *pixels = calloc(640u * 800u, sizeof(*pixels));
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
    frame->text.base.cells[49 * 80 + 79].glyph_index = 65;
    frame->text.base.cells[49 * 80 + 79].glyph_bank = 1;
    frame->text.base.cells[49 * 80 + 79].foreground = 1;
    frame->text.secondary_font[65 * 16 + 7] = 1;
    CHECK(kvm_window_frame_validate(frame) == LIB_STATUS_OK);
    CHECK(kvm_window_frame_size(frame, &width, &height) && width == 640 && height == 400);
    CHECK(kvm_window_render_frame(frame, pixels, width, height, &valid, &changed));
    CHECK(pixels[399 * 640 + 639] == 0x123456);
    CHECK(!kvm_window_render_frame(frame, pixels, width, height, &valid, &changed));
    frame->text.base.text_rows = 43;
    CHECK(kvm_window_frame_size(frame, &width, &height) && height == 344);
    frame->text.base.text_rows = 51;
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
    CHECK(frontend_window_dos_frame(&video, frame) == ERROR_NO_DATA && !frame->valid);
    CHECK(run16_console_video_data(&video, 1, 4, dib + 4, 4) == ERROR_SUCCESS);
    CHECK(frontend_window_dos_frame(&video, frame) == ERROR_SUCCESS);
    CHECK(frame->image.width == 9 && frame->image.stride == 9 && frame->image.height == 2);
    CHECK(frame->image.pixels[0] == 1 && frame->image.pixels[8] == 1);
    CHECK(frame->image.pixels[9] == 0 && frame->image.pixels[10] == 1);
    CHECK(frame->image.palette[1] == 0x2468ac);
    CHECK(run16_console_video_begin(&video, 2, &description) == ERROR_SUCCESS);
    CHECK(frontend_window_dos_frame(&video, frame) == ERROR_SUCCESS); /* Retain complete frame. */
    CHECK(run16_console_video_text(&video, 3) == ERROR_SUCCESS);
    CHECK(frontend_window_dos_frame(&video, frame) == ERROR_NO_DATA && !frame->valid);
    description.width = 3; description.depth = 8;
    CHECK(run16_console_video_begin(&video, 4, &description) == ERROR_SUCCESS);
    CHECK(run16_console_video_data(&video, 4, 0, dib, 8) == ERROR_SUCCESS);
    CHECK(frontend_window_dos_frame(&video, frame) == ERROR_SUCCESS);
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
        CHECK(frontend_window_dos_frame(&video,frame)==ERROR_NO_DATA);
        CHECK(!run16_console_video_data(&video,1,16384,payload+16384,text.bytes-16384));
        CHECK(!frontend_window_dos_frame(&video,frame) && !frame->graphics);
        CHECK(frame->text.base.text_rows==50 && frame->text.base.font_height==8);
        valid=LIB_FALSE;
        CHECK(kvm_window_render_frame(frame,pixels,640,400,&valid,&changed));
        CHECK(pixels[399*640+639]==0x123456);
        CHECK(!run16_console_video_begin(&video,2,&text));
        style->font_height=33;
        CHECK(run16_console_video_data(&video,2,0,payload,text.bytes)==ERROR_INVALID_DATA);
        CHECK(video.published_serial==1 && !frontend_window_dos_frame(&video,frame));
        style->font_height=8;style->cursor_start=INT32_MAX;
        CHECK(!run16_console_video_begin(&video,3,&text));
        CHECK(run16_console_video_data(&video,3,0,payload,text.bytes)==ERROR_INVALID_DATA);
        CHECK(video.published_serial==1);
        style->cursor_start=0;text.height=25;text.bytes=sizeof(*style)+160*25;
        style->font_height=20;style->fonts[0][65][19]=1;style->attribute_font_select=0;
        payload[sizeof(*style)]=65;payload[sizeof(*style)+1]=9;
        CHECK(!run16_console_video_begin(&video,4,&text));
        CHECK(!run16_console_video_data(&video,4,0,payload,text.bytes));
        CHECK(!frontend_window_dos_frame(&video,frame) && frame->graphics);
        CHECK(frame->image.height==500 && frame->image.pixels[19*640+7]==9);
        --text.bytes;
        CHECK(run16_console_video_begin(&video,5,&text)==ERROR_INVALID_DATA);
        free(payload);run16_console_video_dispose(&video);
        puts("PASS copied DOS text: complete-frame publication, 80x50 dual font, tall glyph fallback and malformed frame retention");
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
    CHECK(frame->image.width == 8 && frame->image.height == 14);
    CHECK(frame->image.palette[frame->image.pixels[0]] == 0x050a0f);
    CHECK(frame->image.palette[frame->image.pixels[13 * 8]] == 0x151617);
    native.screen.dwCursorPosition.X = 5000; native.screen.dwCursorPosition.Y = 299;
    native.cursor.bVisible = TRUE;
    CHECK(frontend_window_native_frame(&native, native_cells, 5001u * 300u, frame) == ERROR_SUCCESS);
    CHECK(frame->image.palette[frame->image.pixels[13 * 8]] == (0x151617 ^ 0xffffff));
    {
        POINT pointer={0,0};unsigned arrow_pixels=0,index;
        CHAR_INFO original=native_cells[5001 * 299 + 5000];
        DWORD pointer_error=frontend_window_native_frame_pointer(&native,native_cells,5001u*300u,&pointer,frame);
        if(pointer_error)fprintf(stderr,"pointer conversion error=%lu last=%lu\n",pointer_error,GetLastError());
        CHECK(!pointer_error);
        for(index=0;index<8u*14u;++index) {
            DWORD color=frame->image.palette[frame->image.pixels[index]];
            if(color==0 || color==0xffffff)++arrow_pixels;
        }
        CHECK(arrow_pixels && !memcmp(&original,&native_cells[5001 * 299 + 5000],sizeof(original)));
        CHECK(!frontend_window_native_frame(&native,native_cells,5001u*300u,frame));
        CHECK(frame->image.palette[frame->image.pixels[0]]==0x050a0f);
        puts("PASS native pointer is a clipped OS arrow on copied raster; Console cells and text caret remain independent");
    }
    CHECK(frontend_window_native_frame(&native, native_cells, 1, frame) == ERROR_INVALID_DATA && !frame->valid);
    /* Large backing buffers and large visible viewports are distinct. */
    native.cursor.bVisible = FALSE;
    native.screen.srWindow.Left = native.screen.srWindow.Top = 0;
    native.screen.srWindow.Right = 159;
    native.screen.srWindow.Bottom = 53;
    native_cells[5001 * 53 + 159].Char.UnicodeChar = L' ';
    native_cells[5001 * 53 + 159].Attributes = 0x23 | COMMON_LVB_UNDERSCORE;
    CHECK(frontend_window_native_frame(&native, native_cells, 5001u * 300u, frame) == ERROR_SUCCESS);
    CHECK(frame->valid && frame->image.width == 1280 && frame->image.height == 756);
    CHECK(frame->image.palette[frame->image.pixels[755 * 1280 + 1279]] == 0x151617);
    native.screen.srWindow.Right = 160;
    CHECK(frontend_window_native_frame(&native, native_cells, 5001u * 300u, frame) == ERROR_NOT_SUPPORTED && !frame->valid);
    native.screen.srWindow.Right = 159;
    native.screen.srWindow.Bottom = 54;
    CHECK(frontend_window_native_frame(&native, native_cells, 5001u * 300u, frame) == ERROR_NOT_SUPPORTED && !frame->valid);
    /* This is the library transport boundary, not proof of oversized Window
     * support. Never mask it by reducing the source font or cropping cells. */
    puts("PASS nominal native raster; oversized Window transport rejected without font shrinking");
    free(native_cells);
    free(pixels); free(frame);
    puts("PASS nxvm x86 closure: 80x50/43, font banks, indexed graphics, CAF make/break, invalid lifecycle");
    puts("PASS frontend frames: partial/complete/TEXT, monochrome/top-down/padding, native viewport/scrollback/underline/cursor");
    return 0;
}
