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

int segment_len(const struct window *w, int segment);

bool all_done(const struct window *w);

void set_received(struct window *w, int segment, const char *data);