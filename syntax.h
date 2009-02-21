#ifndef SYNTAX_H
#define SYNTAX_H

#include "common.h"
#include "list.h"
#include "color.h"

#define SYNTAX_NODE_CONTEXT	0
#define SYNTAX_NODE_PATTERN	1
#define SYNTAX_NODE_WORD	2

#define SYNTAX_FLAG_ICASE	(1 << 0)
#define SYNTAX_FLAG_HEREDOC	(1 << 1)

#define WORD_HASH_SIZE 64

struct syntax_any {
	char *name;
	struct hl_color *color;
	int type;
	unsigned int flags;
};

struct hash_word {
	struct hash_word *next;
	char *word;
};

struct syntax_word {
	struct syntax_any any;

	struct hash_word *hash[WORD_HASH_SIZE];
};

struct syntax_pattern {
	struct syntax_any any;

	char *pattern;

	regex_t regex;
};

struct syntax_context {
	struct syntax_any any;

	struct hl_color *scolor;
	struct hl_color *ecolor;

	int nr_nodes;
	char *spattern;
	char *epattern;
	union syntax_node **nodes;

	regex_t sregex;
	regex_t eregex;
};

union syntax_node {
	struct syntax_any any;
	struct syntax_word word;
	struct syntax_pattern pattern;
	struct syntax_context context;
};

enum syntax_node_specifier {
	/* For all nodes */
	SPECIFIER_NORMAL,

	/* For contexts only.  Example: sh.command:start, sh.command:end */
	SPECIFIER_CONTEXT_START,
	SPECIFIER_CONTEXT_END,
};

struct syntax_join_item {
	union syntax_node *node;
	enum syntax_node_specifier specifier;
};

struct syntax_join {
	char *name;
	struct syntax_join_item *items;
	int nr_items;
};

struct syntax {
	struct list_head node;
	char *name;
	struct syntax_context *root;
};

extern union syntax_node **syntax_nodes;
extern int nr_syntax_nodes;

extern struct syntax_join *syntax_joins;
extern int nr_syntax_joins;

void syn_begin(char **args);
void syn_end(char **args);
void syn_addw(char **args);
void syn_addr(char **args);
void syn_addc(char **args);
void syn_connect(char **args);
void syn_join(char **args);

unsigned int str_hash(const char *str);
struct syntax *find_syntax(const char *name);

static inline unsigned char get_syntax_node_idx(const union syntax_node *node)
{
	unsigned char idx;
	for (idx = 0; syntax_nodes[idx] != node; idx++)
		;
	return idx;
}

static inline union syntax_node *idx_to_syntax_node(unsigned char idx)
{
	return syntax_nodes[idx];
}

static inline struct syntax_context *syntax_get_default_context(const struct syntax *syn)
{
	return syn->root;
}

#endif
