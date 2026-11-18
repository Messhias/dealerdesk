/*
 * screenshot.h - render console text buffer to a PNG file
 */
#ifndef SCREENSHOT_H
#define SCREENSHOT_H

/* Write terminal text (ANSI replayed into a grid) to a PNG path. Returns 0. */
int screenshot_write_png(const char *text, const char *path);

/*
 * Same as screenshot_write_png, but skips the write when the visible grid
 * matches *last_hash (duplicate frame). On a real write, updates *last_hash.
 * Returns 0 written, 1 skipped duplicate, -1 error.
 */
int screenshot_write_png_unique(const char *text, const char *path,
                                unsigned long *last_hash);

#endif
