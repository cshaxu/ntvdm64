/* Build seam for libvterm 0.3.3. Compile its original screen translation unit
 * once, unchanged. The finite extension below uses its private representation;
 * see libvterm-import.json and the S9 source-recovery ledger. */
#include <screen.c>
#include "native_terminal_screen.h"

int ntkvm_vterm_replace_screen(VTermScreen *screen,int rows,int columns,
    const VTermScreenCell *cells,VTermPos cursor)
{
    int row,column;VTermPos previous;VTermRect changed={0,rows,0,columns};
    if(!screen || !cells || rows!=screen->rows || columns!=screen->cols ||
       cursor.row<0 || cursor.row>=rows || cursor.col<0 || cursor.col>=columns)
        return 0;
    /* Complete validation before changing the live screen, including the last
     * column of a wide glyph. Continuation cells are covered by their leader. */
    for(row=0;row<rows;++row)for(column=0;column<columns;) {
        int width=cells[row*columns+column].width;
        if((width!=1 && width!=2) || column+width>columns)return 0;
        column+=width;
    }
    vterm_screen_flush_damage(screen);
    for(row=0;row<rows;++row) {
        /* Imported Console rows have ordinary single-cell geometry. */
        screen->state->lineinfo[row]=(VTermLineInfo){0};
        for(column=0;column<columns;) {
            const VTermScreenCell *src=cells+row*columns+column;
            ScreenCell *dst=getcell(screen,row,column);
            memset(dst,0,sizeof(*dst));
            memcpy(dst->chars,src->chars,sizeof(dst->chars));
            /* Same conversion as upstream resize's sb_popline restoration. */
            dst->pen.bold=src->attrs.bold;
            dst->pen.underline=src->attrs.underline;
            dst->pen.italic=src->attrs.italic;
            dst->pen.blink=src->attrs.blink;
            dst->pen.reverse=src->attrs.reverse ^ screen->global_reverse;
            dst->pen.conceal=src->attrs.conceal;
            dst->pen.strike=src->attrs.strike;
            dst->pen.font=src->attrs.font;
            dst->pen.small=src->attrs.small;
            dst->pen.baseline=src->attrs.baseline;
            dst->pen.fg=src->fg;dst->pen.bg=src->bg;
            if(src->width==2) {
                dst[1]=*dst;dst[1].chars[0]=(uint32_t)-1;
            }
            column+=src->width;
        }
    }
    previous=screen->state->pos;screen->state->pos=cursor;
    screen->state->at_phantom=0;
    screen->state->combine_pos=(VTermPos){-1,-1};
    damagerect(screen,changed);
    movecursor(cursor,previous,screen->state->mode.cursor_visible,screen);
    return 1;
}
