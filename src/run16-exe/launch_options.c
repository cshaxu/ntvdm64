#include "launch_options.h"

static const wchar_t *skip_space(const wchar_t *text)
{
    while (*text == L' ' || *text == L'\t') ++text;
    return text;
}

const wchar_t *run16_command_line_tail(const wchar_t *command_line)
{
    const wchar_t *text;
    if (!command_line) return NULL;
    text = skip_space(command_line);
    if (*text == L'"') {
        ++text;
        while (*text && *text != L'"') ++text;
        if (*text == L'"') ++text;
    } else {
        while (*text && *text != L' ' && *text != L'\t') ++text;
    }
    return skip_space(text);
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
    if (!command) return 0;
    target = skip_space(command);
    for (;;) {
        if (prefix_token(target, L"--wait")) {
            if (options->wait) return 0;
            options->wait = 1;
            target = skip_space(target + 6);
        } else if (prefix_token(target, L"--win16-style") ||
                   prefix_token(target, L"--win16-style-all")) return 0;
        else break;
    }
    if (!*target) return 0;
    options->command = target;
    return 1;
}
