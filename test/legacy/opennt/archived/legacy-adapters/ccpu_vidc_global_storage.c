/*
 * Build-context overlay for OpenNT ccpu386/localfm.c.
 *
 * localfm.c establishes these historical vector/storage globals, but its
 * abbreviated include sequence no longer supplies complete SAS/VIDC types to
 * clang-cl. This translation unit preserves the declarations absent from the
 * selected CCPU archive while using the generated-header order already present
 * in sascdef.c. That archive already owns Sas, so it is deliberately omitted
 * here to preserve its one-definition boundary. This file adds storage only;
 * it implements no CPU, video, port, or host behavior.
 */
#include "insignia.h"
#include "host_inc.h"
#include "host_def.h"
#include "fpu_c.h"
#include "pigreg_c.h"
#include "univer_c.h"
#define CPU_PRIVATE
#include "cpu4.h"
#include "gdpvar.h"
#include "sas.h"
#include "evidgen.h"

IHP Gdp;
struct CpuVector Cpu;
struct VideoVector Video;
