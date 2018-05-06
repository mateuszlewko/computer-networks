#include "window.h"

struct window create_window(FILE *file, int total_length) {
    struct window w;
    memset(w.received, 0, SEGMENTS_CNT);
    w.total_length = total_length;
    w.total_segments = (total_length + SEGMENT_LEN - 1) / SEGMENT_LEN;
    w.curr_segment_saved = -1;
    w.file = file;

    return w;
}

int segment_len(const struct window *w, int segment) {
    return MIN(w->total_length - segment * SEGMENT_LEN, SEGMENT_LEN);
}

bool all_done(const struct window *w) {
    return w->curr_segment_saved >= w->total_segments - 1;
}

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

void print_progress (double percentage)
{  // source: https://stackoverflow.com/questions/14539867/how-to-display-a-progress-indicator-in-pure-c-c-cout-printf
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf ("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush (stdout);
}

void set_received(struct window *w, int segment, const char *data) {
    if (segment > w->curr_segment_saved + SEGMENTS_CNT
        || segment <= w->curr_segment_saved)
        return;

    int pos = segment % SEGMENTS_CNT;

    if (w->received[pos])
        return;

    w->received[pos] = true;
    int len = segment_len(w, segment);
    memcpy(w->buffers[pos], data, len);
    
    int next_pos = (w->curr_segment_saved + 1) % SEGMENTS_CNT;
    while (w->received[next_pos]) {
        w->received[next_pos] = false;
        fwrite(w->buffers[next_pos], sizeof(char), 
               segment_len(w, w->curr_segment_saved + 1), w->file);
        
        print_progress((double)(w->curr_segment_saved + 2) / w->total_segments);

        w->curr_segment_saved++;
        next_pos = (next_pos + 1) % SEGMENTS_CNT;
    }
}