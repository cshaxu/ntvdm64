#include "wow_input_group_bindings.h"
#include <stdio.h>

static unsigned errors,releases;
#define CHECK(x) do { if(!(x)){++errors;printf("FAIL %d\n",__LINE__);} } while(0)
static void WINAPI release_queue(wow_task_order_queue *q,wow_task_order_thread *t)
{
    (void)t;CHECK(q->cThreads!=0);--q->cThreads;++releases;
}
static void link_thread(wow_input_desktop *d,wow_task_order_thread *t,
    wow_task_order_process *p,UINT flags)
{
    t->rpdesk=d;t->ppi=p;t->TIF_flags=flags;
    t->PtiLink.Flink=&d->PtiList;t->PtiLink.Blink=d->PtiList.Blink;
    d->PtiList.Blink->Flink=&t->PtiLink;d->PtiList.Blink=&t->PtiLink;
}
int __cdecl main(void)
{
    wow_input_desktop d={0};
    wow_task_order_process p={0},other={0};
    wow_task_order_thread a={0},b={0},helper={0},foreign={0},newcomer={0};
    wow_task_order_queue q={0},old={0};
    wow_input_attachment edge={0},bridge={0};
    BOOL changed=FALSE;
    d.PtiList.Flink=d.PtiList.Blink=&d.PtiList;d.destroy_queue=release_queue;
    /* Reverse order forces Recalc2's fixed-point iteration after discovery. */
    link_thread(&d,&foreign,&other,TIF_16BIT);
    link_thread(&d,&helper,&p,0);
    link_thread(&d,&b,&p,TIF_16BIT);
    link_thread(&d,&a,&p,TIF_16BIT);
    a.pqAttach=&q;q.cThreads=1;
    b.pqAttach=&old;old.cThreads=1;
    edge.pti1=&b;edge.pti2=&helper;d.attachments=&edge;
    Recalc2(&q,&a);
    CHECK(b.ptdb==NULL && b.pqAttach==&q); /* Before original InsertTask. */
    CHECK(helper.pqAttach==&q && foreign.pqAttach==NULL);
    CHECK(q.cThreads==3 && old.cThreads==0 && releases==1);
    Recalc2(&q,&a);CHECK(q.cThreads==3 && releases==1);
    AddAttachment(&a,&q,&changed);CHECK(!changed);
    /* Explicit cross-process edge, not implicit WOW process grouping. */
    bridge.pti1=&helper;bridge.pti2=&foreign;edge.paiNext=&bridge;
    Recalc2(&q,&a);CHECK(foreign.pqAttach==&q && q.cThreads==4);
    /* Rebuild caller-owned provisional queue assignments after b leaves. */
    b.PtiLink.Blink->Flink=b.PtiLink.Flink;
    b.PtiLink.Flink->Blink=b.PtiLink.Blink;
    d.attachments=NULL;a.pqAttach=&q;helper.pqAttach=foreign.pqAttach=NULL;
    q.cThreads=1;
    link_thread(&d,&newcomer,&p,TIF_16BIT);
    Recalc2(&q,&a);
    CHECK(newcomer.pqAttach==&q && q.cThreads==2);
    CHECK(helper.pqAttach==NULL && foreign.pqAttach==NULL);
    printf("WOW_INPUT_MEMBERSHIP errors=%u original_fixed_point=1 pre_tdb=1 explicit_edge=1 release=1 rejoin=1\n",errors);
    return errors!=0;
}
