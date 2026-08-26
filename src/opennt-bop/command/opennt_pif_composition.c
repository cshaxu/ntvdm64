/* DIVERGENCE(BOP-DIV-032): OpenNT's softpc.new PIF parser and COMMAND
 * cmdpif.c share process-global state but use incompatible historical
 * PIF_DATA layouts.  The exact parser remains in this owner-family mirror;
 * its required modern binding is private overlay code so no host component
 * acquires an invalid dependency on COMMAND. */
#include "opennt-bop-overlay/command/opennt_pif_composition.c"
