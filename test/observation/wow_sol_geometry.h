#ifndef WOW_SOL_GEOMETRY_H
#define WOW_SOL_GEOMETRY_H

/* A partial result needs each short axis to match the observed host limit.
 * Caller must also establish matching DPI-awareness contexts. */
static int sol_geometry_result(int cw, int ch, int ow, int oh,
    int maxw, int maxh, int same_context)
{
    int width_short = cw < 585;
    int height_short = ch < 384;
    if (!width_short && !height_short) return 0;
    if (!same_context) return 2;
    if (width_short && (ow != maxw || maxw >= 585 + ow - cw)) return 2;
    if (height_short && (oh != maxh || maxh >= 384 + oh - ch)) return 2;
    return 1;
}
#endif
