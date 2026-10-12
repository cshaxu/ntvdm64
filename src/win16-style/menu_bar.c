#include "menu_bar.h"
#include <stdlib.h>
#include <string.h>

void win16_style_bar_free(win16_style_bar *bar)
{
    UINT i;
    if(bar->items)for(i=0;i<bar->count;++i)free(bar->items[i].text);
    free(bar->items);free(bar->rects);memset(bar,0,sizeof(*bar));
}

BOOL win16_style_bar_read(HMENU menu,HFONT font,UINT dpi,POINT origin,int width,win16_style_bar *bar)
{
    int count=GetMenuItemCount(menu);UINT i,n=0;SIZE *sizes=NULL;UINT *flags=NULL;HDC dc=NULL;BOOL ok=FALSE;
    memset(bar,0,sizeof(*bar));if(count<0 || width<=0)return FALSE;
    if(!count)return TRUE;
    bar->count=(UINT)count;
    bar->items=calloc(bar->count,sizeof(*bar->items));bar->rects=calloc(bar->count,sizeof(*bar->rects));
    sizes=calloc(bar->count,sizeof(*sizes));flags=calloc(bar->count,sizeof(*flags));
    dc=CreateCompatibleDC(NULL);
    if(!bar->items || !bar->rects || !sizes || !flags || !dc)goto done;
    for(i=0;i<bar->count;++i){
        MENUITEMINFOW info={sizeof(info)};
        info.fMask=MIIM_STRING|MIIM_FTYPE|MIIM_STATE|MIIM_ID|MIIM_SUBMENU|MIIM_BITMAP;
        if(!GetMenuItemInfoW(menu,i,TRUE,&info) || (info.fType&MFT_SEPARATOR))goto done;
        /* NTUSER's old-UI MDI merge has system + restore only. Modern USER
         * adds minimize/close; omit those presentation cells without editing
         * the application's actual HMENU or its native command routing. */
        if(info.hbmpItem==HBMMENU_MBAR_MINIMIZE || info.hbmpItem==HBMMENU_MBAR_MINIMIZE_D ||
            info.hbmpItem==HBMMENU_MBAR_CLOSE || info.hbmpItem==HBMMENU_MBAR_CLOSE_D)continue;
        if(info.hbmpItem && info.hbmpItem!=HBMMENU_SYSTEM && info.hbmpItem!=HBMMENU_MBAR_RESTORE)goto done;
        /* USER marks its merged restore item owner-drawn as well as giving it
         * the standard bitmap identity. Arbitrary app owner-draw stays outside
         * this string/standard-control snapshot contract. */
        if((info.fType&MFT_OWNERDRAW) && !info.hbmpItem)goto done;
        bar->items[n].text=calloc((size_t)info.cch+1,sizeof(WCHAR));
        if(!bar->items[n].text)goto done;
        info.dwTypeData=bar->items[n].text;++info.cch;
        if(!GetMenuItemInfoW(menu,i,TRUE,&info))goto done;
        bar->items[n].state=info.fState;bar->items[n].id=info.wID;bar->items[n].popup=info.hSubMenu;
        bar->items[n].control=info.hbmpItem;flags[n]=info.fType;
        if(info.hbmpItem){sizes[n].cx=MulDiv(19,(int)dpi,96);sizes[n].cy=MulDiv(18,(int)dpi,96);
            if(info.hbmpItem==HBMMENU_MBAR_RESTORE)flags[n]|=MFT_RIGHTJUSTIFY;}
        else if(!win16_style_menu_measure(dc,font,dpi,info.dwTypeData,WIN16_STYLE_MENU_BAR,&sizes[n]))goto done;
        ++n;
    }
    bar->count=n;
    ok=win16_style_menu_arrange(origin,width,sizes,flags,bar->count,bar->rects,&bar->height);
done:
    if(dc)DeleteDC(dc);free(sizes);free(flags);
    if(!ok)win16_style_bar_free(bar);
    return ok;
}

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
