/*
 * screenshot.h - render console text buffer to a PNG file
 */
#ifndef SCREENSHOT_H
#define SCREENSHOT_H

/* Write UTF-8/ASCII terminal text (ANSI stripped) to a PNG path. Returns 0. */
int screenshot_write_png(const char *text, const char *path);

#endif
