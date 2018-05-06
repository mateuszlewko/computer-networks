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
    
    if (pos == w->curr_segment_saved + 1) {
        w->received[++w->curr_segment_saved] = false;
        fwrite(w->buffers[pos], sizeof(char), len, w->file);
    }
}