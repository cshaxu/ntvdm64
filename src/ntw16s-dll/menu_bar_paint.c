#include "menu_bar.h"

void win16_style_bar_paint(const win16_style_bar *bar,HDC dc,HFONT font,UINT dpi,const win16_style_palette *palette)
{
    UINT i;
    for(i=0;i<bar->count;++i){
        UINT state=0;
        if(bar->items[i].state&MFS_HILITE)state|=ODS_SELECTED;
        if(bar->items[i].state&MFS_DISABLED)state|=ODS_DISABLED;
        if(bar->items[i].control)win16_style_merged_control(dc,bar->rects[i],bar->items[i].control==HBMMENU_SYSTEM,
            (state&ODS_SELECTED)!=0,max(1,MulDiv(1,(int)dpi,96)),palette);
        else win16_style_menu_text(dc,&bar->rects[i],font,dpi,palette,bar->items[i].text,state,WIN16_STYLE_MENU_BAR,FALSE);
    }
}
