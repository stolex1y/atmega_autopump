#include "menu.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "lcd1602.h"
#include "coordinates.h"

enum menu_state {
    MAIN_MENU = 0,
    MENU_LIST,
    ITEM_SETTING,
    MENU_STATE_LEN
};

struct menu_item_list {
    struct menu_item* items;
    size_t items_len;
    size_t curr_item;
};

struct menu {
    struct lcd1602* lcd;
    struct menu_item_list* item_lists;
    size_t curr_item_list;
    size_t item_lists_len;
    struct coordinates upper_left;
    struct coordinates bottom_right;
    enum menu_state state;
};

static void menu_update_item_screen(const struct menu* menu);
static void menu_next_item_list(struct menu* menu);
static void menu_previous_item_list(struct menu* menu);
static void menu_next_item(struct menu* menu);
static void menu_previous_item(struct menu* menu);
static struct menu_item* menu_current_item(const struct menu* menu);
static struct menu_item_list* menu_current_item_list(const struct menu* menu);
static void menu_item_setting_handle(const struct menu_item* curr_item, enum menu_item_setting_state state);

struct menu* menu_create(struct lcd1602* const lcd, const struct coordinates upper_left,
        const struct coordinates bottom_right, const size_t item_lists_len) {
    if (!lcd) return NULL;
    struct menu* const menu = malloc(sizeof(struct menu));
    *menu = (struct menu) {
            .lcd = lcd,
            .item_lists = malloc(sizeof(struct menu_item_list) * item_lists_len),
            .curr_item_list = 0,
            .item_lists_len = item_lists_len,
            .upper_left = upper_left,
            .bottom_right = bottom_right,
            .state = MAIN_MENU
    };
    return menu;
}

void menu_next(struct menu* const menu) {
    struct menu_item* current_item;
    switch (menu->state) {
        case MAIN_MENU:
            menu_next_item_list(menu);
            break;
        case MENU_LIST:
            menu_next_item(menu);
            break;
        case ITEM_SETTING:
            current_item = menu_current_item(menu);
            menu_item_setting_handle(current_item, ITEM_STTNG_UPDATE);
            break;
        default:
            break;
    }
}

void menu_previous(struct menu* const menu) {
    struct menu_item* current_item;
    switch (menu->state) {
        case MAIN_MENU:
            menu_previous_item_list(menu);
            break;
        case MENU_LIST:
            menu_previous_item(menu);
            break;
        case ITEM_SETTING:
            current_item = menu_current_item(menu);
            menu_item_setting_handle(current_item, ITEM_STTNG_UPDATE);
            break;
        default:
            break;
    }
}

void menu_next_state(struct menu* const menu) {
    if (!menu) return;
    struct menu_item* current_item = menu_current_item(menu);
    switch (menu->state) {
        case MAIN_MENU:
            menu->state = MENU_LIST;
            break;
        case MENU_LIST:
            if (!current_item->readonly) {
                menu->state = ITEM_SETTING;
                menu_item_setting_handle(current_item, ITEM_STTNG_START);
            }
            break;
        case ITEM_SETTING:
            menu_item_setting_handle(current_item, ITEM_STTNG_NEXT);
            break;
        default:
            break;
    }
}

void menu_previous_state(struct menu* const menu) {
    if (!menu) return;
    struct menu_item* const current_item = menu_current_item(menu);
    switch (menu->state) {
        case MENU_LIST:
            menu->state = MAIN_MENU;
            break;
        case ITEM_SETTING:
            menu_item_setting_handle(current_item, ITEM_STTNG_CANCEL);
            menu->state = MENU_LIST;
        default:
            break;
    }
}

void menu_item_list_init(struct menu* const menu, const size_t item_list, const size_t items_len) {
    if (!menu) return;
    if (item_list >= menu->item_lists_len) return;
    struct menu_item* const items = malloc(sizeof(struct menu_item) * items_len);
    menu->item_lists[item_list] = (struct menu_item_list) {
            .items = items,
            .items_len = items_len,
            .curr_item = 0
    };
}

void menu_item_set(struct menu* const menu, const size_t item_list, const size_t item_i, const struct menu_item item) {
    if (!menu || item_list >= menu->item_lists_len) return;
    struct menu_item_list* const curr_list = menu->item_lists + item_list;
    if (item_i >= curr_list->items_len) return;
    curr_list->items[item_i] = item;
}

struct menu_item menu_item_create(const char* key, const char* value, const size_t menu_width, const bool readonly) {
    const size_t key_len = strlen(key);
    const size_t value_len = strlen(value);
    size_t key_width, value_width;
    if (key_len + value_len <= menu_width - 2) {
        key_width = key_len;
        value_width = value_len;
    } else {
        key_width = menu_width / 2 - 2;
        value_width = menu_width - key_width - 1;
    }
    char* const k = calloc(key_width + 1, sizeof(char));
    memcpy(k, key, key_width);
    char* const v = calloc(value_width + 1, sizeof(char));
    memcpy(v, value, value_width);
    menu_item_setting_handler** const handlers = readonly ? NULL : malloc(ITEM_STTNG_LEN * sizeof(menu_item_setting_handler*));
    return (struct menu_item) {
            .key = k,
            .value = v,
            .readonly = readonly,
            .handlers = handlers
    };
}

size_t menu_get_curr_list_index(const struct menu* const menu) {
    if (!menu) return -1;
    return menu->curr_item_list;
}

size_t menu_get_curr_item_index(const struct menu* const menu) {
    if (!menu) return -1;
    return menu_current_item_list(menu)->curr_item;
}

void menu_set_key(struct menu* const menu, const char* const new_key) {
    if (!menu) return;
    struct menu_item* const curr_item = menu_current_item(menu);
    if (curr_item->readonly) return;
    char* const curr_key = curr_item->key;
    memcpy(curr_key, new_key, strlen(curr_key));
    menu_update_item_screen(menu);
}

const char* menu_get_key(const struct menu* const menu) {
    if (!menu) return NULL;
    const char* const curr_key = menu_current_item(menu)->key;
    return curr_key;
}

void menu_set_value(struct menu* const menu, const char* const new_value) {
    if (!menu) return;
    struct menu_item* const curr_item = menu_current_item(menu);
    if (curr_item->readonly) return;
    char* const curr_value = curr_item->value;
    memcpy(curr_value, new_value, strlen(curr_value));
    menu_update_item_screen(menu);
}

const char* menu_get_value(const struct menu* const menu) {
    if (!menu) return NULL;
    const char* const curr_value = menu_current_item(menu)->value;
    return curr_value;
}

void menu_item_setting_end(struct menu* const menu) {
    if (!menu || menu->state != ITEM_SETTING) return;
    struct menu_item* const curr_item = menu_current_item(menu);
    if (curr_item->readonly) return;
    menu_item_setting_handle(curr_item, ITEM_STTNG_END);
    menu->state = MENU_LIST;
}

static void menu_item_setting_handle(const struct menu_item* const curr_item, const enum menu_item_setting_state state) {
    if (!curr_item || curr_item->readonly) return;
    if (curr_item->handlers && curr_item->handlers[state])
        curr_item->handlers[state]();
}

static struct menu_item_list* menu_current_item_list(const struct menu* const menu) {
    if (!menu) return NULL;
    return menu->item_lists + menu->curr_item_list;
}

static struct menu_item* menu_current_item(const struct menu* const menu) {
    if (!menu) return NULL;
    const struct menu_item_list* const current_list = menu_current_item_list(menu);
    if (!current_list) return NULL;
    return current_list->items + current_list->curr_item;
}

static void menu_next_item_list(struct menu* const menu) {
    if (!menu) return;
    struct menu_item_list* const current = menu_current_item_list(menu);
    current->curr_item = 0;
    menu->curr_item_list = (menu->curr_item_list + 1) % menu->item_lists_len;
    menu_update_screen(menu);
}

static void menu_previous_item_list(struct menu* const menu) {
    if (!menu) return;
    struct menu_item_list* const current = menu_current_item_list(menu);
    current->curr_item = 0;
    if (menu->curr_item_list == 0)
        menu->curr_item_list = menu->item_lists_len - 1;
    else
        menu->curr_item_list--;
    menu_update_screen(menu);
}

static void menu_next_item(struct menu* const menu) {
    if (!menu) return;
    struct menu_item_list* const curr_list = menu_current_item_list(menu);
    if (curr_list->curr_item + 1 >= curr_list->items_len) return;
    curr_list->curr_item++;
    menu_update_screen(menu);
}

static void menu_previous_item(struct menu* const menu) {
    if (!menu) return;
    struct menu_item_list* const curr_list = menu_current_item_list(menu);
    if (curr_list->curr_item == 0) return;
    curr_list->curr_item--;
    menu_update_screen(menu);
}

static size_t menu_width(const struct menu* const menu) {
    if (!menu) return 0;
    return menu->bottom_right.x - menu->upper_left.x + 1;
}

static size_t menu_height(const struct menu* const menu) {
    if (!menu) return 0;
    return menu->bottom_right.y - menu->upper_left.y + 1;
}

static void menu_clear_screen(const struct menu* const menu) {
    lcd1602_clear_rect(menu->lcd, menu->upper_left, menu->bottom_right);
}

static void menu_item_print(const struct menu* const menu, const size_t item_index) {
    if (!menu) return;
    struct menu_item_list* const curr_item_list = menu_current_item_list(menu);
    if (item_index >= curr_item_list->items_len) return;

    lcd1602_send_string(menu->lcd, curr_item_list->items[item_index].key);
    lcd1602_send_string(menu->lcd, ": ");
    lcd1602_send_string(menu->lcd, curr_item_list->items[item_index].value);
}

static void menu_update_item_screen(const struct menu* const menu) {
    if (!menu) return;
    lcd1602_clear_rect(
            menu->lcd,
            menu->upper_left,
            coordinates_create(menu->upper_left.x + menu_width(menu), menu->upper_left.y)
    );
    const size_t item_index = menu_get_curr_item_index(menu);
    lcd1602_set_cursor_pos(menu->lcd, menu->upper_left);
    menu_item_print(menu, item_index);
}

void menu_update_screen(const struct menu* const menu) {
    if (!menu) return;
    menu_clear_screen(menu);
    size_t height = menu_height(menu);
    size_t item_index = 0;
    struct menu_item_list* curr_item_list = menu_current_item_list(menu);
    if (height <= curr_item_list->items_len)
        item_index = menu_get_curr_item_index(menu);
    for (size_t y = menu->upper_left.y; y <= menu->bottom_right.y; y++) {
        lcd1602_set_cursor_pos(menu->lcd, coordinates_create(menu->upper_left.x, y));
        menu_item_print(menu, item_index);
        if (item_index + 1 < curr_item_list->items_len)
            item_index++;
        else
            break;
    }
}


