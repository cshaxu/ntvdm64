#include "wow_input_group_bindings.h"
#include <stdio.h>

static unsigned errors,releases;
static unsigned allocations;
static BOOL allocation_failure;
static wow_task_order_queue allocated;
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
static wow_task_order_queue *WINAPI allocate_queue(wow_input_desktop *d)
{
    (void)d;++allocations;
    return allocation_failure ? NULL : &allocated;
}
static void queue_selection(void)
{
    wow_input_desktop d={0};
    wow_task_order_process p={0},other={0};
    wow_task_order_thread a={0},b={0},c={0};
    wow_task_order_queue shared={0},single={0};
    d.PtiList.Flink=d.PtiList.Blink=&d.PtiList;
    d.allocate_queue=allocate_queue;d.destroy_queue=release_queue;
    link_thread(&d,&a,&p,TIF_16BIT);link_thread(&d,&b,&p,TIF_16BIT);
    link_thread(&d,&c,&other,0);
    shared.cThreads=2;single.cThreads=1;
    a.pq=b.pq=&shared;c.pq=&single;
    allocation_failure=TRUE;
    RecalcThreadAttachment(&a);
    CHECK(allocations==1 && !a.pqAttach && !b.pqAttach && !c.pqAttach);
    CHECK(shared.cThreads==2 && single.cThreads==1);
    allocation_failure=FALSE;
    RecalcThreadAttachment(&a);
    CHECK(allocations==2 && a.pqAttach==&allocated && b.pqAttach==&allocated);
    CHECK(allocated.cThreads==2 && c.pqAttach==&single);
    CHECK(shared.cThreads==2 && single.cThreads==1);
    RecalcThreadAttachment(&a);CHECK(allocations==2);
    /* Original failure is not transactional: a preceding singly owned
     * thread keeps its provisional assignment when a later allocation fails. */
    a.pqAttach=b.pqAttach=c.pqAttach=NULL;
    c.PtiLink.Blink->Flink=c.PtiLink.Flink;
    c.PtiLink.Flink->Blink=c.PtiLink.Blink;
    c.PtiLink.Flink=d.PtiList.Flink;c.PtiLink.Blink=&d.PtiList;
    d.PtiList.Flink->Blink=&c.PtiLink;d.PtiList.Flink=&c.PtiLink;
    allocation_failure=TRUE;
    RecalcThreadAttachment(&a);
    CHECK(allocations==3 && c.pqAttach==&single);
    CHECK(!a.pqAttach && !b.pqAttach && single.cThreads==1);
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
    queue_selection();
    printf("WOW_INPUT_MEMBERSHIP errors=%u original_fixed_point=1 pre_tdb=1 explicit_edge=1 release=1 rejoin=1 queue_selection=1 allocation_failure=1 partial_failure=1\n",errors);
    return errors!=0;
}
