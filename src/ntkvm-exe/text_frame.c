#include "text_frame.h"
#include "interface/console_video.h"

/* The copied text protocol is bounded at 160x96/font32. Retain the page as
 * text so the Window, rather than a baked image, owns cursor blinking. */
BOOL frontend_text_frame_prepare(const kvm_window_text_frame *fonts,
    const kvm_text_cell *cells,size_t cell_count,
    const frontend_text_extension *extension,BOOL cursor_phase,
    kvm_window_frame *output)
{
    unsigned row,column,columns,rows;
    if(output)output->valid=0;
    if(!fonts || !cells || !output) {
        SetLastError(ERROR_INVALID_PARAMETER);return FALSE;
    }
    columns=fonts->base.text_columns;rows=fonts->base.text_rows;
    if(!columns || !rows || columns>KVM_TEXT_COLUMNS || rows>KVM_TEXT_ROWS ||
        !fonts->base.font_height || fonts->base.font_height>KVM_WINDOW_FONT_HEIGHT) {
        SetLastError(ERROR_NOT_SUPPORTED);return FALSE;
    }
    if(cell_count<(size_t)columns*rows) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);return FALSE;
    }
    if(extension && extension->cell_styles && !extension->cell_style_stride) {
        SetLastError(ERROR_INVALID_PARAMETER);return FALSE;
    }
    output->graphics=0;output->text=*fonts;
    output->text.base.cursor_phase=(lib_bool)!!cursor_phase;
    output->text.secondary_cursor_visible=extension && extension->cursor_visible;
    output->text.secondary_cursor_top=extension ? extension->cursor_top : 0;
    output->text.secondary_cursor_bottom=extension ? extension->cursor_bottom : 0;
    for(row=0;row<rows;++row)for(column=0;column<columns;++column) {
        size_t source=(size_t)row*columns+column;
        size_t target=(size_t)row*KVM_TEXT_COLUMNS+column;
        output->text.base.cells[target]=cells[source];
        output->text.styles[target]=extension && extension->cell_styles ?
            extension->cell_styles[source*extension->cell_style_stride] : 0;
        if(output->text.styles[target]&~CONSOLE_TEXT_STYLE_MASK) {
            SetLastError(ERROR_INVALID_DATA);return FALSE;
        }
    }
    output->valid=1;
    if(kvm_window_frame_validate(output)!=LIB_STATUS_OK) {
        output->valid=0;SetLastError(ERROR_INVALID_DATA);return FALSE;
    }
    return TRUE;
}
