/* Link-only probe: retain the original handler object without calling it. */
extern void unexpected_int(void);

int main(void)
{
    void (*volatile handler)(void) = unexpected_int;
    return handler == 0;
}
