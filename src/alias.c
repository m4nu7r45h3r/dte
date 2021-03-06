#include "alias.h"
#include "ptr-array.h"
#include "common.h"
#include "error.h"
#include "editor.h"
#include "completion.h"
#include "command.h"

typedef struct {
    char *name;
    char *value;
} CommandAlias;

static PointerArray aliases = PTR_ARRAY_INIT;

static int validate_alias_name(const char *name)
{
    for (size_t i = 0; name[i]; i++) {
        char ch = name[i];
        if (!ascii_isalnum(ch) && ch != '-' && ch != '_') {
            return 0;
        }
    }
    return !!name[0];
}

void add_alias(const char *name, const char *value)
{
    CommandAlias *alias;

    if (!validate_alias_name(name)) {
        error_msg("Invalid alias name '%s'", name);
        return;
    }
    if (find_command(commands, name)) {
        error_msg("Can't replace existing command %s with an alias", name);
        return;
    }

    // Replace existing alias
    for (size_t i = 0; i < aliases.count; i++) {
        alias = aliases.ptrs[i];
        if (streq(alias->name, name)) {
            free(alias->value);
            alias->value = xstrdup(value);
            return;
        }
    }

    alias = xnew(CommandAlias, 1);
    alias->name = xstrdup(name);
    alias->value = xstrdup(value);
    ptr_array_add(&aliases, alias);

    if (editor.status != EDITOR_INITIALIZING) {
        sort_aliases();
    }
}

static int alias_cmp(const void *ap, const void *bp)
{
    const CommandAlias *a = *(const CommandAlias **)ap;
    const CommandAlias *b = *(const CommandAlias **)bp;
    return strcmp(a->name, b->name);
}

void sort_aliases(void)
{
    if (aliases.count > 1) {
        BUG_ON(!aliases.ptrs);
        qsort(aliases.ptrs, aliases.count, sizeof(*aliases.ptrs), alias_cmp);
    }
}

const char *find_alias(const char *name)
{
    for (size_t i = 0; i < aliases.count; i++) {
        const CommandAlias *alias = aliases.ptrs[i];
        if (streq(alias->name, name)) {
            return alias->value;
        }
    }
    return NULL;
}

void collect_aliases(const char *prefix)
{
    for (size_t i = 0; i < aliases.count; i++) {
        CommandAlias *alias = aliases.ptrs[i];
        if (str_has_prefix(alias->name, prefix)) {
            add_completion(xstrdup(alias->name));
        }
    }
}
