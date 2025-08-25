#include "menu_icon.h"
#include "display.h"
#include "theme.h"
#include "tile.h"

#define ICON_SIZE 60

//pseudo code for now until sd card read working
// 60x60 char array where 1 is pixel on and 0 is pixel off
static char* read_icon_from_sd(const char *filename) {

    // FILE *file = fopen(filename, "r");
    // if (!file) return NULL;
    // char *data = malloc(ICON_SIZE * ICON_SIZE);
    // fread(data, 1, ICON_SIZE * ICON_SIZE, file);
    // fclose(file);
    // return data;
    
    return NULL;
}

void draw_menu_icon(int tile_x, int tile_y, const char *filename) {
    if (!filename) {
        return;
    }
    
    char *icon_data = read_icon_from_sd(filename);
    if (!icon_data) {
        return;
    }

    int px, py;
    tile_to_pixels(tile_x, tile_y, &px, &py);

    for (int y = 0; y < ICON_SIZE; y++) {
        for (int x = 0; x < ICON_SIZE; x++) {
            uint16_t pixel_x = px + x;
            uint16_t pixel_y = py + y;
            
            if (pixel_x < ST7789V_LCD_PIXEL_WIDTH && pixel_y < ST7789V_LCD_PIXEL_HEIGHT) {
                char pixel_value = icon_data[y * ICON_SIZE + x];
                
                if (pixel_value) {
                    // can be optimised to set display window then dma dump the array.
                    st7789v_write_pixel(pixel_x, pixel_y, current_theme.fg_colour);
                }
            }
        }
    }

    free(icon_data);
}