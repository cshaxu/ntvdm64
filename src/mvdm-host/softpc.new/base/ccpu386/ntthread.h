extern void ccpu386InitThreadStuff();
extern void ccpu386foundnewthread();
extern void ccpu386newthread();
extern void ccpu386exitthread();
extern jmp_buf *ccpu386SimulatePtr();
extern void ccpu386Unsimulate();
extern jmp_buf *ccpu386ThrdExptnPtr();
extern void ccpu386GotoThrdExptnPt();

extern void ccpu386AbandonSimulate(jmp_buf *frame);
#define CCPU_COMPLETED_INSTRUCTION 2
extern void ccpu386CompleteInstruction(void);
