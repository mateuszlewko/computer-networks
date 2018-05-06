#pragma once

#include "config.h"
#include "utils.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

struct window {
    bool received[SEGMENTS_CNT];
    char buffers[SEGMENTS_CNT][SEGMENT_LEN];
    int total_length;
    int total_segments;
    int curr_segment_saved;
    FILE *file;
};

struct window create_window(FILE *file, int total_length);

inline int segment_len(const struct window *w, int segment) {
    return MIN(w->total_length - segment * SEGMENT_LEN, SEGMENT_LEN);
}

inline bool all_done(const struct window *w) {
    return w->curr_segment_saved == w->total_segments;
}

void set_received(struct window *w, int segment, const char *data);