/* Temporary presentation of native string popup items. Native tracking,
 * command IDs, enable/check state and menu ownership are unchanged. */
#include "menu_items.h"
#include <stdlib.h>

struct win16_style_menu_item {
    struct win16_style_menu_item *next;
    HMENU menu;
    UINT type;
    ULONG_PTR data;
    WCHAR *text;
    HBITMAP bitmap;
    BOOL submenu;
    unsigned depth;
};

static win16_style_menu_item *find(win16_style_menu_item *items,ULONG_PTR data)
{
    for(;items;items=items->next)if((ULONG_PTR)items==data)return items;
    return NULL;
}

BOOL win16_style_menu_char(win16_style_menu_item *items,HMENU menu,WCHAR key,LRESULT *result)
{
    int i,count=GetMenuItemCount(menu),selected=-1,first=-1,next=-1,matches=0;BOOL owned=FALSE;
    CharUpperBuffW(&key,1);
    for(i=0;i<count;++i){MENUITEMINFOW info={sizeof(info)};info.fMask=MIIM_STATE;
        if(GetMenuItemInfoW(menu,(UINT)i,TRUE,&info) && (info.fState&MFS_HILITE))selected=i;}
    for(i=0;i<count;++i){
        MENUITEMINFOW info={sizeof(info)};win16_style_menu_item *item;const WCHAR *text;
        info.fMask=MIIM_DATA|MIIM_STATE|MIIM_FTYPE;
        if(!GetMenuItemInfoW(menu,(UINT)i,TRUE,&info) || !(item=find(items,info.dwItemData)))continue;
        owned=TRUE;
        if((info.fState&MFS_DISABLED) || (info.fType&MFT_SEPARATOR))continue;
        for(text=item->text;*text && *text!=L'\t';++text)if(*text==L'&' && text[1]){
            WCHAR letter=*++text;if(letter==L'&')continue;CharUpperBuffW(&letter,1);
            if(letter!=key)continue;
            if(first<0)first=i;if(next<0 && i>selected)next=i;++matches;break;
        }
    }
    if(!owned)return FALSE;
    *result=!matches?MAKELRESULT(0,MNC_IGNORE):
        MAKELRESULT(matches==1?first:next<0?first:next,matches==1?MNC_EXECUTE:MNC_SELECT);
    return TRUE;
}

static void restore_items(HMENU menu,unsigned depth,BOOL by_depth,win16_style_menu_item **items)
{
    while(*items){
        win16_style_menu_item *item=*items;int i,count=GetMenuItemCount(item->menu);
        if((menu && item->menu!=menu) || (by_depth && item->depth!=depth)){items=&item->next;continue;}
        for(i=0;i<count;++i){
            MENUITEMINFOW info={sizeof(info)};info.fMask=MIIM_DATA|MIIM_FTYPE|MIIM_BITMAP;
            if(GetMenuItemInfoW(item->menu,(UINT)i,TRUE,&info) && info.dwItemData==(ULONG_PTR)item){
                /* Do not restore state/IDs/text changed by the application.
                 * Remove only our owner-draw bit and our borrowed-data marker. */
                info.dwItemData=item->data;
                info.fType=(info.fType&~MFT_OWNERDRAW)|(item->type&MFT_OWNERDRAW);
                if(!info.hbmpItem)info.hbmpItem=item->bitmap;
                SetMenuItemInfoW(item->menu,(UINT)i,TRUE,&info);break;
            }
        }
        *items=item->next;free(item->text);free(item);
    }
}

void win16_style_menu_restore(win16_style_menu_item **items)
{
    restore_items(NULL,0,FALSE,items);
}
void win16_style_menu_restore_popup(HMENU menu,win16_style_menu_item **items)
{restore_items(menu,0,FALSE,items);}
void win16_style_menu_restore_depth(unsigned depth,win16_style_menu_item **items)
{restore_items(NULL,depth,TRUE,items);}

BOOL win16_style_menu_prepare(HMENU menu,win16_style_menu_item **items,unsigned depth)
{
    int i,count=GetMenuItemCount(menu);
    if(count<0)return FALSE;
    for(i=0;i<count;++i){
        MENUITEMINFOW info={sizeof(info)};win16_style_menu_item *item;
        info.fMask=MIIM_FTYPE|MIIM_DATA|MIIM_STRING|MIIM_SUBMENU|MIIM_BITMAP;
        if(!GetMenuItemInfoW(menu,(UINT)i,TRUE,&info))return FALSE;
        if((info.fType&MFT_OWNERDRAW) || (info.hbmpItem &&
            info.hbmpItem!=HBMMENU_POPUP_CLOSE && info.hbmpItem!=HBMMENU_POPUP_RESTORE &&
            info.hbmpItem!=HBMMENU_POPUP_MINIMIZE && info.hbmpItem!=HBMMENU_POPUP_MAXIMIZE))continue;
        item=calloc(1,sizeof(*item));if(!item)return FALSE;
        item->text=calloc((size_t)info.cch+1,sizeof(WCHAR));
        if(!item->text){free(item);return FALSE;}
        item->menu=menu;item->type=info.fType;item->data=info.dwItemData;item->submenu=info.hSubMenu!=NULL;
        item->bitmap=info.hbmpItem;item->depth=depth;
        info.dwTypeData=item->text;++info.cch;
        if(!GetMenuItemInfoW(menu,(UINT)i,TRUE,&info)){free(item->text);free(item);return FALSE;}
        info.fMask=MIIM_FTYPE|MIIM_DATA|MIIM_BITMAP;info.fType|=MFT_OWNERDRAW;
        info.dwItemData=(ULONG_PTR)item;info.hbmpItem=NULL;
        if(!SetMenuItemInfoW(menu,(UINT)i,TRUE,&info)){free(item->text);free(item);return FALSE;}
        item->next=*items;*items=item;
    }
    return TRUE;
}

BOOL win16_style_menu_draw(win16_style_menu_item *items,const DRAWITEMSTRUCT *draw,
    HFONT font,UINT dpi,const win16_style_palette *palette)
{
    win16_style_menu_item *item;
    if(draw->CtlType!=ODT_MENU || !(item=find(items,draw->itemData)))return FALSE;
    return win16_style_menu_text(draw->hDC,&draw->rcItem,font,dpi,palette,item->text,draw->itemState,
        (item->type&MFT_SEPARATOR)?WIN16_STYLE_MENU_SEPARATOR:WIN16_STYLE_MENU_ROW,item->submenu);
}

BOOL win16_style_menu_size(win16_style_menu_item *items,MEASUREITEMSTRUCT *measure,HFONT font,UINT dpi)
{
    win16_style_menu_item *item;HDC dc;SIZE size;BOOL ok;
    if(measure->CtlType!=ODT_MENU || !(item=find(items,measure->itemData)))return FALSE;
    dc=CreateCompatibleDC(NULL);if(!dc)return FALSE;
    ok=win16_style_menu_measure(dc,font,dpi,item->text,
        (item->type&MFT_SEPARATOR)?WIN16_STYLE_MENU_SEPARATOR:WIN16_STYLE_MENU_ROW,&size);
    DeleteDC(dc);if(ok){measure->itemWidth=size.cx;measure->itemHeight=size.cy;}return ok;
}
