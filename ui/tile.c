#include <stdbool.h>
#include "tile.h"

static bool dirty[TILE_ROWS][TILE_COLS];

void mark_tile_dirty(int tile_x, int tile_y) {
    if (tile_x >= 0 && tile_x < TILE_COLS && tile_y >= 0 && tile_y < TILE_ROWS) {
        dirty[tile_y][tile_x] = true;
    }
}

void mark_all_tiles_dirty(void) {
    for (int y = 0; y < TILE_ROWS; y++) {
        for (int x = 0; x < TILE_COLS; x++) {
            dirty[y][x] = true;
        }
    }
}

void flush_dirty_tiles(void (*draw_tile)(int tx, int ty)) {
    for (int y = 0; y < TILE_ROWS; y++) {
        for (int x = 0; x < TILE_COLS; x++) {
            if (dirty[y][x]) {
                draw_tile(x, y);
                dirty[y][x] = false;
            }
        }
    }
}
