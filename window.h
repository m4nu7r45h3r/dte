#ifndef WINDOW_H
#define WINDOW_H

#include "buffer.h"

struct window {
	struct ptr_array views;
	struct frame *frame;

	// Current view
	struct view *view;

	// Coordinates and size of entire window including tabbar and status line
	int x, y;
	int w, h;

	// Coordinates and size of editable area
	int edit_x, edit_y;
	int edit_w, edit_h;

	struct {
		int width;
		int first;
		int last;
	} line_numbers;

	int first_tab_idx;

	bool update_tabbar;
};

extern struct window *window;
extern struct ptr_array windows;

struct window *new_window(void);
struct view *window_add_buffer(struct window *w, struct buffer *b);
struct view *window_open_empty_buffer(struct window *w);
struct view *window_open_buffer(struct window *w, const char *filename, bool must_exist, const char *encoding);
struct view *window_get_view(struct window *w, struct buffer *b);
struct view *window_find_unclosable_view(struct window *w, bool (*can_close)(struct view *));
void window_remove_views(struct window *w);
void window_free(struct window *w);
void remove_view(struct view *v);
void close_current_view(void);
void set_view(struct view *v);
struct view *open_new_file(void);
struct view *open_file(const char *filename, const char *encoding);
void open_files(char **filenames, const char *encoding);
void mark_buffer_tabbars_changed(struct buffer *b);
int vertical_tabbar_width(struct window *win);
void calculate_line_numbers(struct window *win);
void set_window_coordinates(struct window *win, int x, int y);
void set_window_size(struct window *win, int w, int h);
int window_get_scroll_margin(struct window *w);

#endif
