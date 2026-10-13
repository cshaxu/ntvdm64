#include "launch_options.h"

static const wchar_t *skip_space(const wchar_t *text)
{
    while (*text == L' ' || *text == L'\t') ++text;
    return text;
}

static int prefix_token(const wchar_t *text, const wchar_t *token)
{
    size_t length = wcslen(token);
    return wcsncmp(text, token, length) == 0 &&
        (!text[length] || text[length] == L' ' || text[length] == L'\t');
}

int run16_parse_launch_options(const wchar_t *command, run16_launch_options *options)
{
    const wchar_t *target;
    if (!options) return 0;
    options->command = NULL;
    options->wait = 0;
    options->window_style = WINDOW_STYLE_OFF;
    options->window_style_explicit = 0;
    if (!command) return 0;
    target = skip_space(command);
    for (;;) {
        if (prefix_token(target, L"--wait")) {
            if (options->wait) return 0;
            options->wait = 1;
            target = skip_space(target + 6);
        } else if (prefix_token(target, L"--win16-style") ||
                   prefix_token(target, L"--win16-style-all")) {
            int all = prefix_token(target, L"--win16-style-all");
            if (options->window_style_explicit) return 0;
            options->window_style_explicit = 1;
            options->window_style = all ? WINDOW_STYLE_ALL : WINDOW_STYLE_WIN16;
            target = skip_space(target + (all ? 17 : 13));
        } else break;
    }
    if (!*target) return 0;
    options->command = target;
    return 1;
}
