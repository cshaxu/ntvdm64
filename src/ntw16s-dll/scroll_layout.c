#include "scroll_layout.h"
#include <limits.h>
#include <stdint.h>
#include <string.h>

/* Independently expressed geometry from pinned USER 18:073C..081F;
 * original evidence and the modern-width extension are recorded in
 * docs/etc/evidence/m0-t445-s4-scrollbar-fidelity-audit.md.
 * This is not imported executable code or NT4's proportional-thumb branch. */
BOOL win16_scroll_measure(const RECT *bar,BOOL vertical,
    const win16_scroll_metrics *m,LONG minimum,LONG maximum,LONG position,
    win16_scroll_layout *l)
{
    int64_t start,end,length,cross,arrow,origin,travel,thumb;
    int64_t range=(int64_t)maximum-minimum,offset;
    if(!bar || !m || !l || m->arrow<1 || m->thumb<1 || m->border<0 || range<0)return FALSE;
    start=vertical?bar->top:bar->left;end=vertical?bar->bottom:bar->right;
    length=end-start;cross=vertical?(int64_t)bar->right-bar->left:(int64_t)bar->bottom-bar->top;
    if(length<2*(int64_t)m->border || length<1 || length>INT_MAX || cross<1 || cross>INT_MAX)return FALSE;
    /* 18:07AA..07C7: shorten arrows, then subtract the shared border from
     * the minimum thumb origin. Preserve the border in both travel ends. */
    arrow=min(length/2-m->border,m->arrow);
    origin=start+arrow-m->border;
    travel=end-arrow-origin+m->border-m->thumb;
    if(origin<INT_MIN || origin>INT_MAX || travel<INT_MIN || travel>INT_MAX ||
        origin+max((int64_t)0,travel)+m->thumb>INT_MAX)return FALSE;
    position=max(minimum,min(maximum,position));offset=(int64_t)position-minimum;
    /* The original helper rounds multiplication/division. Extend only the
     * arithmetic width for native 32-bit ranges; no signed overflow/wrap. */
    thumb=origin;
    if(range && travel>0)thumb+=(offset*travel+range/2)/range;
    if(thumb<INT_MIN || thumb+m->thumb>INT_MAX)return FALSE;
    memset(l,0,sizeof(*l));l->bar=*bar;l->vertical=vertical;
    l->minimum=minimum;l->maximum=maximum;l->position=position;
    l->minimum_pixel=(int)origin;l->travel=(int)travel;
    l->first=l->last=l->thumb=*bar;
    /* USER 18:03DE..03EE skips the thumb if the region between arrows is
     * smaller than the fixed thumb; do not squeeze it into a modern thumb. */
    l->thumb_fits=length-2*arrow>=m->thumb;
    l->thumb_draggable=length-2*arrow>m->thumb;
    if(vertical){l->first.bottom=(LONG)(start+arrow);l->last.top=(LONG)(end-arrow);
        l->thumb.top=(LONG)thumb;l->thumb.bottom=(LONG)(thumb+m->thumb);}
    else {l->first.right=(LONG)(start+arrow);l->last.left=(LONG)(end-arrow);
        l->thumb.left=(LONG)thumb;l->thumb.right=(LONG)(thumb+m->thumb);}
    return TRUE;
}

LONG win16_scroll_position(const win16_scroll_layout *l,int thumb_origin)
{
    int64_t pixels,range;
    if(!l)return 0;
    if(l->travel<=0)return l->minimum;
    pixels=(int64_t)thumb_origin-l->minimum_pixel;
    pixels=max(0,min((int64_t)l->travel,pixels));
    range=(int64_t)l->maximum-l->minimum;
    return (LONG)(l->minimum+(pixels*range+l->travel/2)/l->travel);
}

win16_scroll_part win16_scroll_hit(const win16_scroll_layout *l,POINT point,UINT disabled)
{
    LONG pixel,thumb_start,thumb_end;
    if(!l || !PtInRect(&l->bar,point) || (disabled&ESB_DISABLE_BOTH)==ESB_DISABLE_BOTH)
        return WIN16_SCROLL_NONE;
    /* USER 18:170B..1792 tests arrows before the overlapping thumb edge;
     * a thumb can be drawn in exactly enough room but cannot be dragged. */
    if(PtInRect(&l->first,point))return disabled&ESB_DISABLE_UP?WIN16_SCROLL_NONE:WIN16_SCROLL_LINE_BEFORE;
    if(PtInRect(&l->last,point))return disabled&ESB_DISABLE_DOWN?WIN16_SCROLL_NONE:WIN16_SCROLL_LINE_AFTER;
    pixel=l->vertical?point.y:point.x;
    thumb_start=l->vertical?l->thumb.top:l->thumb.left;
    thumb_end=l->vertical?l->thumb.bottom:l->thumb.right;
    if(pixel<thumb_start)return WIN16_SCROLL_PAGE_BEFORE;
    if(pixel>=thumb_end)return WIN16_SCROLL_PAGE_AFTER;
    return l->thumb_draggable?WIN16_SCROLL_THUMB:WIN16_SCROLL_NONE;
}
