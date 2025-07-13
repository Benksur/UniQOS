#ifndef TILE_H
#define TILE_H

#define TILE_WIDTH 30
#define TILE_HEIGHT 30
#define NAVBAR_HEIGHT 25
#define TILE_ROWS 9
#define TILE_COLS 8

static inline void tile_to_pixels(int tx, int ty, int* out_x, int* out_y) {
    *out_x = tx * TILE_WIDTH;
    *out_y = NAVBAR_HEIGHT + ty * TILE_HEIGHT;
}

void mark_tile_dirty(int tile_x, int tile_y);
void mark_all_tiles_dirty(void);
void flush_dirty_tiles(void (*draw_tile)(int tx, int ty));

#endif
