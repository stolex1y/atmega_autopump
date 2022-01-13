#ifndef ARDUINO_AUTOPUMP_MENU_H
#define ARDUINO_AUTOPUMP_MENU_H

#include <inttypes.h>
#include <stdio.h>

#include "lcd1602.h"
#include "coordinates.h"

enum menu_state {
    MAIN_MENU = 0,
    MENU_LIST,
    ITEM_SETTING,
    MENU_STATE_LEN
};

enum menu_item_setting_state {
    ITEM_STTNG_START,
    ITEM_STTNG_NEXT,
    ITEM_STTNG_UPDATE,
    ITEM_STTNG_CANCEL,
    ITEM_STTNG_END,
    ITEM_STTNG_LEN
};

typedef void item_setting_handler();

struct menu_item_list {
//    struct menu_item_list* next;
//    struct menu_item_list* previous;
    struct menu_item* items;
    size_t items_len;
    size_t curr_item;
};

struct menu_item {
    char* key;
    char* value;
    bool readonly;
    item_setting_handler** handlers;
//    enum menu_item_setting_state setting_state;
};

struct menu {
    struct lcd1602* lcd;
    struct menu_item_list* item_lists;
    size_t curr_item_list;
    size_t item_lists_len;
//    struct menu_item_list* curr_item_list;
    struct coordinates upper_left;
    struct coordinates bottom_right;
    enum menu_state state;
};

struct menu* menu_create(struct lcd1602* lcd, struct coordinates upper_left,
                         struct coordinates bottom_right, size_t item_lists_len);
//void menu_item_list_add(struct menu* menu, struct menu_item_list* item_list);
void menu_item_list_init(struct menu* menu, size_t item_list, size_t items_len);
void menu_item_set(struct menu* menu, size_t item_list, size_t item_i, struct menu_item item);
struct menu_item menu_item_create(const char* key, const char* value, const size_t menu_width, bool readonly);

void menu_next(struct menu* menu);
void menu_previous(struct menu* menu);
void menu_next_state(struct menu* menu);
void menu_previous_state(struct menu* menu);
void menu_item_setting_end(struct menu* menu);

size_t menu_get_curr_item_index(struct menu* menu);
size_t menu_get_curr_list_index(struct menu* menu);

void menu_set_key(struct menu* menu, char* new_key);
const char* menu_get_key(struct menu* menu);
void menu_set_value(struct menu* menu, char* new_value);
const char* menu_get_value(struct menu* menu);

void menu_item_setting_end(struct menu* menu);

void menu_update_screen(struct menu* menu);

#endif //ARDUINO_AUTOPUMP_MENU_H
