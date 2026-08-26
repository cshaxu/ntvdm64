/* DIVERGENCE(BOP-DIV-103): the COMMAND provider's bounded active-call,
 * source-pointer materialization and non-returning terminal glue has no
 * standalone OpenNT translation-unit counterpart.  It exceeds the mirror
 * threshold, so the matching private overlay owns the body and this mirror
 * path retains only its one-linkage boundary. */
#include "opennt-bop-overlay/command/opennt_command_composition.c"
