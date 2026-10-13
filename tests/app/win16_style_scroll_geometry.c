#include "win16-style/scroll_layout.h"
#include "win16-style/scroll_track.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned checks;
#define CHECK(x) do { ++checks; if(!(x)){fprintf(stderr,"SCROLL GEOMETRY FAIL %d: %s\n",__LINE__,#x);exit(1);} } while(0)
static void tracking_tests(void)
{
    const win16_scroll_metrics metrics={17,17,1};RECT bar={0,0,17,200},limit={-4,-1,21,201};
    win16_scroll_layout l;win16_scroll_track t={0};win16_scroll_actions a;
    POINT point={8,100};
    CHECK(win16_scroll_measure(&bar,TRUE,&metrics,0,100,50,&l));
    CHECK(win16_scroll_begin(&t,&l,0,point,&limit,&a));
    CHECK(t.active && t.part==WIN16_SCROLL_THUMB && t.grab_offset==8 && t.pixel==92);
    CHECK(a.count==1 && a.notices[0].command==SB_THUMBTRACK && a.notices[0].position==50 && !a.timer_ms);
    CHECK(!win16_scroll_begin(&t,&l,0,point,&limit,&a) && t.active && t.pixel==92);
    point.y=130;win16_scroll_motion(&t,&l,0,point,FALSE,&a);
    CHECK(t.pixel==122 && t.position==70 && a.count==1 && a.notices[0].command==SB_THUMBTRACK);
    win16_scroll_motion(&t,&l,0,point,FALSE,&a);
    CHECK(!a.count && !a.repaint && a.timer_ms==WIN16_SCROLL_KEEP_TIMER);
    point.x=-5;win16_scroll_motion(&t,&l,0,point,FALSE,&a);
    CHECK(!t.inside && t.pixel==92 && t.position==50 && a.count==1);
    point.x=8;win16_scroll_motion(&t,&l,0,point,FALSE,&a);
    CHECK(t.inside && t.pixel==122 && t.position==70 && a.count==1);
    win16_scroll_end(&t,TRUE,&a);
    CHECK(!t.active && t.pixel==92 && !a.timer_ms && a.count==2);
    CHECK(a.notices[0].command==SB_THUMBPOSITION && a.notices[0].position==70 && a.notices[1].command==SB_ENDSCROLL);
    win16_scroll_end(&t,FALSE,&a);CHECK(!a.count && !a.timer_ms);
    point.x=8;point.y=5;CHECK(win16_scroll_begin(&t,&l,0,point,&limit,&a));
    CHECK(a.count==1 && a.notices[0].command==SB_LINEUP && a.timer_ms==200);
    win16_scroll_motion(&t,&l,0,point,FALSE,&a);
    CHECK(!a.count && a.timer_ms==WIN16_SCROLL_KEEP_TIMER);
    win16_scroll_motion(&t,&l,0,point,TRUE,&a);
    CHECK(a.count==1 && a.notices[0].command==SB_LINEUP && a.timer_ms==50);
    point.x=30;win16_scroll_motion(&t,&l,0,point,FALSE,&a);
    CHECK(!t.inside && !a.count && !a.timer_ms && a.repaint);
    point.x=8;win16_scroll_motion(&t,&l,0,point,FALSE,&a);
    CHECK(t.inside && a.count==1 && a.timer_ms==50);
    win16_scroll_motion(&t,&l,ESB_DISABLE_UP,point,FALSE,&a);
    CHECK(!t.inside && !a.count && !a.timer_ms);
    win16_scroll_end(&t,FALSE,&a);CHECK(a.count==1 && a.notices[0].command==SB_ENDSCROLL && !t.active);
    CHECK(!win16_scroll_begin(&t,&l,ESB_DISABLE_BOTH,point,&limit,&a) && !t.active);
    point.y=50;CHECK(win16_scroll_begin(&t,&l,0,point,&limit,&a));
    CHECK(a.count==1 && a.notices[0].command==SB_PAGEUP);
    CHECK(win16_scroll_measure(&bar,TRUE,&metrics,0,100,0,&l));
    win16_scroll_motion(&t,&l,0,point,TRUE,&a);
    CHECK(!t.inside && !a.count && !a.timer_ms);
    win16_scroll_end(&t,FALSE,&a);CHECK(!t.active && a.count==1);
    puts("SCROLL TRACK PASS: grab offset, outside/back, original cancel notices, 200/50ms repeat, no motion timer starvation, page-stop, disabled and idempotent end");
}
int main(void)
{
    const win16_scroll_metrics vga={17,17,1};
    win16_scroll_layout l,h;RECT v={10,20,27,220},r={20,10,220,27};int p,length;
    /* Hand-calculated original-profile vectors: 17-pixel arrows/thumb and
     * the one-pixel shared boundary, not native SCROLLBARINFO as an oracle. */
    CHECK(win16_scroll_measure(&v,TRUE,&vga,0,100,0,&l));
    CHECK(l.first.bottom==37 && l.last.top==203 && l.minimum_pixel==36 && l.travel==151);
    CHECK(l.thumb.top==36 && l.thumb.bottom==53 && l.thumb_fits);
    {POINT p1={12,36},p2={12,37},p3={12,53},p4={27,37};
        CHECK(win16_scroll_hit(&l,p1,0)==WIN16_SCROLL_LINE_BEFORE);
        CHECK(win16_scroll_hit(&l,p2,0)==WIN16_SCROLL_THUMB);
        CHECK(win16_scroll_hit(&l,p3,0)==WIN16_SCROLL_PAGE_AFTER);
        CHECK(win16_scroll_hit(&l,p4,0)==WIN16_SCROLL_NONE);
        CHECK(win16_scroll_hit(&l,p1,ESB_DISABLE_UP)==WIN16_SCROLL_NONE);
        CHECK(win16_scroll_hit(&l,p2,ESB_DISABLE_BOTH)==WIN16_SCROLL_NONE);}
    CHECK(win16_scroll_measure(&v,TRUE,&vga,0,100,50,&l));
    CHECK(l.thumb.top==112 && l.thumb.bottom==129);
    CHECK(win16_scroll_measure(&v,TRUE,&vga,0,100,100,&l));
    CHECK(l.thumb.top==187 && l.thumb.bottom==204);
    CHECK(win16_scroll_position(&l,36)==0 && win16_scroll_position(&l,187)==100);
    CHECK(win16_scroll_position(&l,INT_MIN)==0 && win16_scroll_position(&l,INT_MAX)==100);
    for(length=2;length<=400;++length){
        LONG previous=LONG_MIN;v.bottom=v.top+length;r.right=r.left+length;
        for(p=-101;p<=101;++p){
            CHECK(win16_scroll_measure(&v,TRUE,&vga,-100,100,p,&l));
            CHECK(win16_scroll_measure(&r,FALSE,&vga,-100,100,p,&h));
            CHECK(l.first.bottom==h.first.right && l.last.top==h.last.left);
            CHECK(l.thumb.top==h.thumb.left && l.thumb.bottom==h.thumb.right);
            CHECK(l.thumb.bottom-l.thumb.top==17);
            CHECK(l.position>=-100 && l.position<=100);
            CHECK(l.thumb.top>=previous);previous=l.thumb.top;
            CHECK(l.thumb_fits==((l.last.top-l.first.bottom)>=17));
            CHECK(l.thumb_draggable==((l.last.top-l.first.bottom)>17));
        }
        if(l.thumb_fits){
            previous=LONG_MIN;
            for(p=l.minimum_pixel-1;p<=l.minimum_pixel+l.travel+1;++p){
                LONG position=win16_scroll_position(&l,p);
                CHECK(position>=-100 && position<=100 && position>=previous);previous=position;
            }
        }
    }
    v.bottom=220;
    CHECK(win16_scroll_measure(&v,TRUE,&vga,LONG_MIN,LONG_MAX,LONG_MAX,&l));
    CHECK(l.thumb.top==187);
    CHECK(win16_scroll_position(&l,36)==LONG_MIN && win16_scroll_position(&l,187)==LONG_MAX);
    CHECK(win16_scroll_measure(&v,TRUE,&vga,5,5,500,&l) && l.position==5);
    CHECK(win16_scroll_position(&l,100)==5);
    CHECK(!win16_scroll_measure(&v,TRUE,&vga,1,0,0,&l));
    CHECK(!win16_scroll_measure(NULL,TRUE,&vga,0,100,0,&l));
    v.top=LONG_MIN;v.bottom=LONG_MAX;
    CHECK(!win16_scroll_measure(&v,TRUE,&vga,0,100,0,&l));
    tracking_tests();
    printf("SCROLL GEOMETRY PASS %u checks: original vectors, shared borders, short bars, transpose, monotonic mapping, native-width bounds\n",checks);
    return 0;
}
