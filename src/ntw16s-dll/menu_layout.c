#include "menu_layout.h"
#include <limits.h>

BOOL win16_style_menu_arrange(POINT origin,int width,const SIZE *sizes,
    const UINT *flags,UINT count,RECT *items,int *height)
{
    UINT start=0,i,first_right=count;LONGLONG y=origin.y;
    if(!height || width<=0 || count>INT_MAX ||
        (count && (!sizes || !flags || !items)) ||
        (LONGLONG)origin.x+width>LONG_MAX)return FALSE;
    for(i=0;i<count;++i){
        if(sizes[i].cx<=0 || sizes[i].cy<=0)return FALSE;
        if(first_right==count && (flags[i]&MFT_RIGHTJUSTIFY))first_right=i;
    }
    while(start<count){
        UINT end=start,right;int used=0,row_height=0,x=origin.x;
        while(end<count){
            int item_width=min(sizes[end].cx,width);
            if(end>start && ((flags[end]&MFT_MENUBREAK) || item_width>width-used))break;
            used+=item_width;row_height=max(row_height,sizes[end].cy);++end;
        }
        if(y+row_height>LONG_MAX || y+row_height-origin.y>INT_MAX)return FALSE;
        for(i=start;i<end;++i){
            int item_width=min(sizes[i].cx,width);
            SetRect(&items[i],x,(int)y,x+item_width,(int)y+row_height);x+=item_width;
        }
        /* Original MBC_RightJustifyMenu groups every item after the first
         * right-justified item. Apply that group to each arranged row. */
        right=max(start,first_right);
        for(i=right;i<end;++i)OffsetRect(&items[i],width-used,0);
        y+=row_height;start=end;
    }
    *height=(int)(y-origin.y);return TRUE;
}

int win16_style_menu_hit(const RECT *items,UINT count,POINT point)
{
    UINT i;
    if(!items || count>INT_MAX)return -1;
    for(i=0;i<count;++i)if(PtInRect(&items[i],point))return (int)i;
    return -1;
}
