/* DIVERGENCE(BOP-DIV-081): this is the reached, verbatim UMBNotify fragment
 * from nt_umb.c, not that uncomposable translation unit's UMB allocator.
 * It remains an empty notification, never an implicit UMB/device action. */
void UMBNotify(unsigned char code)
{
    /* See BOP-DIV-081: preserve the original empty result under /WX. */
    (void)code;
    return;
}
