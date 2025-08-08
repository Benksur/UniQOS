#include "screen.h"
#include "display.h"
#include "input.h"
#include "tile.h"

static void draw(void) {
    for (int i = 0; i < TILE_ROWS * TILE_COLS; i++) {
        int tx = i % 8;
        int ty = i / 8;
        int px, py;
        tile_to_pixels(tx, ty, &px, &py);
        display_draw_vertical_line(px + TILE_WIDTH - 1, py, py + TILE_HEIGHT,0x39c7);
        display_draw_horizontal_line(px, py + TILE_HEIGHT, px + TILE_WIDTH, 0x39c7);
    }
}

static void draw_tile(int tile_x, int tile_y) {
    int px, py;
    tile_to_pixels(tile_x, tile_y, &px, &py);
}

static void handle_input(int event_type) {}

Page default_page = {
    .draw = draw,
    .draw_tile = draw_tile,
    .handle_input = handle_input
};