/*
 * Historical private include-path compatibility input.
 *
 * OpenNT's idetect.c includes this path in its NTVDM branch, but the fixed
 * upstream snapshot contains no vdm.h and the selected source uses no symbol
 * from it. Deliberately provide no declarations or behavior here.
 */
#ifndef RUNNER_OPENNT_BASE_INC_VDM_H
#define RUNNER_OPENNT_BASE_INC_VDM_H
#endif
