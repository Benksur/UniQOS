#include "menu_row.h"
#include "tile.h"
#include "theme.h"
#include "display.h"
#include <string.h>

// Icon drawing function pointer type
typedef void (*icon_draw_func_t)(int x, int y, uint16_t colour);

// Icon mapping structure
typedef struct {
    const char* name;
    icon_draw_func_t draw_func;
} icon_mapping_t;

static void draw_phone_icon(int x, int y, uint16_t colour) {
    int thickness = 6;   // thickness of the horizontal lines
    int width = 7;      // width of top/bottom lines
    int height = 30;     // total height of the “C”

    // Center the icon in a 60x60 square
    int icon_x = x + (60 - width - thickness) / 2 - 4;
    int icon_y = y + (60 - height) / 2;

    // Top horizontal line (earpiece)
    display_draw_rect(icon_x + thickness, icon_y, width, thickness, colour);

    // Bottom horizontal line (mouthpiece)
    display_draw_rect(icon_x + thickness, icon_y + height - thickness, width, thickness, colour);

    // Vertical connecting line (back of handset)
    display_draw_rect(icon_x + 2, icon_y + 1, thickness - 1, height - 2, colour);

}

static void draw_sms_icon(int x, int y, uint16_t colour) {
    int bubble_width = 30;
    int bubble_height = 20;
    int radius = 7;  // corner radius

    // Center the bubble in 60x60
    int bx = x + (60 - bubble_width) / 2;
    int by = y + (60 - bubble_height) / 2;

    // Draw main rounded rectangle for the bubble
    display_draw_rounded_square(bx, by, bubble_width, bubble_height, radius, colour);

    // Draw the "tail" triangle at bottom-left corner of the bubble
    int tail_width = 6;
    int tail_height = 6;
    display_draw_triangle(
        bx, by + bubble_height,               // bottom-left corner of bubble
        bx + tail_width, by + bubble_height,  // bottom-right of tail
        bx, by + bubble_height - tail_height, // top of tail
        colour
    );
}

static void draw_contacts_icon(int x, int y, uint16_t colour) {
    // Circle for the head
    int head_radius = 8;
    int head_cx = x + 30; // center x in 60x60
    int head_cy = y + 18; // position head in upper part of the square
    display_draw_circle(head_cx, head_cy, head_radius, colour);

    // Triangle for the body
    int body_width = 20;
    int body_height = 15;
    int body_top_x = head_cx - body_width / 2;
    int body_top_y = head_cy + head_radius;
    display_draw_triangle(
        head_cx, body_top_y,
        body_top_x, body_top_y + body_height,
        body_top_x + body_width, body_top_y + body_height,
        colour
    );
}

static void draw_clock_icon(int x, int y, uint16_t colour) {
    int cx = x + 30;   // center x
    int cy = y + 30;   // center y
    int radius = 20;   // radius of clock face

    // Draw the circular clock face
    display_draw_circle(cx, cy, radius, colour);

    // Draw hour hand (pointing roughly to 2 o'clock)
    int hour_length = 12;
    display_draw_line(cx, cy, cx + hour_length, cy - hour_length / 2, colour);

    // Draw minute hand (pointing roughly to 10 minutes)
    int minute_length = 16;
    display_draw_line(cx, cy, cx, cy - minute_length, colour);
}

static void draw_calculator_icon(int x, int y, uint16_t colour) {
    int calc_width = 36;
    int calc_height = 48;
    int radius = 5;

    // Center the calculator in 60x60
    int cx = x + (60 - calc_width) / 2;
    int cy = y + (60 - calc_height) / 2;

    // Draw outer rounded rectangle for calculator body
    display_draw_rounded_square(cx, cy, calc_width, calc_height, radius, colour);

    // Draw screen rectangle at top
    int screen_height = 10;
    display_draw_rect(cx + 4, cy + 4, calc_width - 8, screen_height, colour);

    // Draw buttons grid (3x4)
    int btn_rows = 3;
    int btn_cols = 3;
    int btn_size = 6;
    int btn_spacing = 4;
    int btn_start_y = cy + 4 + screen_height + 4;
    int btn_start_x = cx + 4;

    for (int r = 0; r < btn_rows; r++) {
        for (int c = 0; c < btn_cols; c++) {
            int bx = btn_start_x + c * (btn_size + btn_spacing);
            int by = btn_start_y + r * (btn_size + btn_spacing);
            display_draw_rect(bx, by, btn_size, btn_size, colour);
        }
    }
}

static void draw_calendar_icon(int x, int y, uint16_t colour) {
    int width = 40;
    int height = 40;
    int radius = 5;

    // Center the calendar in 60x60
    int cx = x + (60 - width) / 2;
    int cy = y + (60 - height) / 2;

    // Calendar body
    display_draw_rounded_square(cx, cy, width, height, radius, colour);

    // Top "binder" rectangles
    int binder_width = 6;
    int binder_height = 4;
    display_draw_rect(cx + 4, cy - 2, binder_width, binder_height, colour);
    display_draw_rect(cx + width - 4 - binder_width, cy - 2, binder_width, binder_height, colour);

    // Grid for days (3x3 small squares)
    int grid_rows = 3;
    int grid_cols = 3;
    int cell_size = 6;
    int spacing = 4;
    int start_x = cx + 4;
    int start_y = cy + 8;

    for (int r = 0; r < grid_rows; r++) {
        for (int c = 0; c < grid_cols; c++) {
            int gx = start_x + c * (cell_size + spacing);
            int gy = start_y + r * (cell_size + spacing);
            display_draw_rect(gx, gy, cell_size, cell_size, colour);
        }
    }
}

static void draw_settings_icon(int x, int y, uint16_t colour) {
    // Center the sliders in 60x60
    int cx = x + 30;
    int cy = y + 30;
    
    // Draw three horizontal slider tracks
    int track_width = 30;
    int track_height = 2;
    int spacing = 10;
    
    // Top slider track
    display_fill_rect(cx - track_width/2, cy - spacing, track_width, track_height, colour);
    // Middle slider track  
    display_fill_rect(cx - track_width/2, cy, track_width, track_height, colour);
    // Bottom slider track
    display_fill_rect(cx - track_width/2, cy + spacing, track_width, track_height, colour);
    
    // Draw slider knobs (circles) at different positions
    int knob_radius = 4;
    
    // Top knob (left position)
    display_fill_circle(cx - 8, cy - spacing + 1, knob_radius, colour);
    // Middle knob (center position)
    display_fill_circle(cx, cy + 1, knob_radius, colour);
    // Bottom knob (right position)
    display_fill_circle(cx + 8, cy + spacing + 1, knob_radius, colour);
}

// Icon mapping array
static const icon_mapping_t icon_mappings[] = {
    {"Phone", draw_phone_icon},
    {"SMS", draw_sms_icon},
    {"Contacts", draw_contacts_icon},
    {"Clock", draw_clock_icon},
    {"Calculator", draw_calculator_icon},
    {"Calendar", draw_calendar_icon},
    {"Settings", draw_settings_icon},
    {NULL, NULL}  // Sentinel value
};

// Helper function to find and draw an icon
static void draw_icon_for_text(const char* text, int x, int y, uint16_t colour) {
    for (int i = 0; icon_mappings[i].name != NULL; i++) {
        if (strcmp(text, icon_mappings[i].name) == 0) {
            icon_mappings[i].draw_func(x, y, colour);
            return;
        }
    }
    // Default icon if no match found
    display_draw_rounded_square(x + 5, y + 5, 50, 50, 5, colour);
}



void draw_menu_row(int tile_y, int selected, const char* text)
{
    int px, py;
    tile_to_pixels(0, tile_y, &px, &py);
    int width = TILE_WIDTH * TILE_COLS;
    int height = TILE_HEIGHT * 2;
    
    if (selected)
    {
        display_fill_rect(px, py, width, height, current_theme.fg_colour);
        display_draw_string(px + 60, py + 10, text, current_theme.bg_colour, current_theme.fg_colour, 2);
        draw_icon_for_text(text, px, py, current_theme.bg_colour);
    } else {
        display_fill_rect(px, py, width, height, current_theme.bg_colour);
        display_draw_string(px + 60, py + 10, text, current_theme.text_colour, current_theme.bg_colour, 2);
        draw_icon_for_text(text, px, py, current_theme.fg_colour);
    }

    display_draw_horizontal_line(px, py, px + width, current_theme.grid_colour);
}

void draw_empty_row(int tile_y) {
    int px, py;
    tile_to_pixels(0, tile_y, &px, &py);
    int width = TILE_WIDTH * TILE_COLS;
    int height = TILE_HEIGHT * 2;

    display_fill_rect(px, py, width, height, current_theme.bg_colour);
}
