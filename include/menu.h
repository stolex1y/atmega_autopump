#ifndef ARDUINO_AUTOPUMP_MENU_H
#define ARDUINO_AUTOPUMP_MENU_H

#include <inttypes.h>
#include <stdio.h>

#include "lcd1602.h"
#include "coordinates.h"

typedef void menu_item_setting_handler();

enum menu_item_setting_state {
    ITEM_STTNG_START,
    ITEM_STTNG_NEXT,
    ITEM_STTNG_UPDATE,
    ITEM_STTNG_CANCEL,
    ITEM_STTNG_END,
    ITEM_STTNG_LEN
};

struct menu;
struct menu_item_list;

struct menu_item {
    char* key;
    char* value;
    bool readonly;
    menu_item_setting_handler** handlers;
};

struct menu* menu_create(struct lcd1602* lcd, struct coordinates upper_left,
                         struct coordinates bottom_right, size_t item_lists_len);
void menu_item_list_init(struct menu* menu, size_t item_list, size_t items_len);
void menu_item_set(struct menu* menu, size_t item_list, size_t item_i, struct menu_item item);
struct menu_item menu_item_create(const char* key, const char* value, size_t menu_width, bool readonly);

void menu_next(struct menu* menu);
void menu_previous(struct menu* menu);
void menu_next_state(struct menu* menu);
void menu_previous_state(struct menu* menu);
void menu_item_setting_end(struct menu* menu);

size_t menu_get_curr_item_index(const struct menu* menu);
size_t menu_get_curr_list_index(const struct menu* menu);

void menu_set_key(struct menu* menu, const char* new_key);
const char* menu_get_key(const struct menu* menu);
void menu_set_value(struct menu* menu, const char* new_value);
const char* menu_get_value(const struct menu* menu);

void menu_update_screen(const struct menu* menu);

#endif //ARDUINO_AUTOPUMP_MENU_H
