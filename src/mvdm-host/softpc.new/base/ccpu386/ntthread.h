extern void ccpu386InitThreadStuff();
extern void ccpu386foundnewthread();
extern void ccpu386newthread();
extern void ccpu386exitthread();
extern jmp_buf *ccpu386SimulatePtr();
extern void ccpu386Unsimulate();
/* DIVERGENCE(MVDM-HOST-DIV-348): exact abandoned native jump frame only. */
extern void ccpu386AbandonSimulate(jmp_buf *frame);
extern jmp_buf *ccpu386ThrdExptnPtr();
extern void ccpu386GotoThrdExptnPt();

