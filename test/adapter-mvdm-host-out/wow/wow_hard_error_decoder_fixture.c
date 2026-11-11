#include <stdio.h>

#include "wow_hard_error_dialog.h"

#define SEB_OK 1u
#define SEB_CANCEL 2u
#define SEB_RETRY 5u
#define SEB_CLOSE 8u
#define SEB_DEFAULT 0x8000u

static int check_case(const char *name, const WORD descriptors[3], UINT count,
    UINT first, UINT second, UINT third, UINT default_slot, UINT cancel_slot)
{
    mvdm_wow_hard_error_buttons buttons;

    if (!mvdm_wow_hard_error_decode_buttons(descriptors, &buttons) ||
        buttons.count != count || buttons.default_slot != default_slot ||
        buttons.cancel_slot != cancel_slot ||
        (count > 0u && buttons.source_slots[0] != first) ||
        (count > 1u && buttons.source_slots[1] != second) ||
        (count > 2u && buttons.source_slots[2] != third)) {
        printf("FAIL %s count=%u default=%u cancel=%u slots=%u,%u,%u\n", name,
            buttons.count, buttons.default_slot, buttons.cancel_slot,
            buttons.source_slots[0], buttons.source_slots[1],
            buttons.source_slots[2]);
        return 0;
    }
    printf("PASS %s count=%u default=%u cancel=%u\n", name, buttons.count,
        buttons.default_slot, buttons.cancel_slot);
    return 1;
}

int main(void)
{
    static const WORD sparse[] = { SEB_CANCEL, 0u, SEB_RETRY | SEB_DEFAULT };
    static const WORD later_default[] = {
        SEB_OK | SEB_DEFAULT, SEB_CANCEL | SEB_DEFAULT, SEB_CLOSE
    };
    static const WORD invalid[] = { 9u, SEB_CLOSE, SEB_RETRY | SEB_DEFAULT };
    static const WORD no_default[] = { 0u, SEB_CLOSE, SEB_RETRY };
    static const WORD empty[] = { 0u, 9u, 0u };
    mvdm_wow_hard_error_buttons buttons;

    if (!check_case("sparse", sparse, 2u, 1u, 3u, 0u, 3u, 1u) ||
        !check_case("later-default", later_default, 3u, 1u, 2u, 3u, 2u, 2u) ||
        !check_case("invalid-omitted", invalid, 2u, 2u, 3u, 0u, 3u, 0u) ||
        !check_case("implicit-first-default", no_default, 2u, 2u, 3u, 0u, 2u, 0u) ||
        mvdm_wow_hard_error_decode_buttons(empty, &buttons)) return 1;
    puts("PASS source-slot/default/cancel decoder contract");
    return 0;
}
