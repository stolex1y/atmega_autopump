#include "../include/autopump.h"

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "../include/lcd1602.h"
#include "../include/coordinates.h"
#include "../include/menu.h"
#include "../include/encoder.h"
#include "../include/date.h"
#include "../include/eeprom.h"
#include "../include/plant.h"
#include "../include/uptime.h"
#include "../include/timer_millis.h"
#include "../include/gpio.h"

#define EEPROM_KEY_ADDR 0
#define EEPROM_KEY "Autopump"
#define EEPROM_KEY_LEN 9
#define EEPROM_ADDR_START EEPROM_KEY_LEN
#define EEPROM_NAME_SIZE 18
#define DATE_STR_SIZE 9

static struct menu* menu;
static struct encoder* encoder;

struct autopump {
    struct plant* plant;
    struct timer_millis* timer;
};

static struct {
    struct plant* plants;
    struct autopump* pumps;
    size_t len;
} autopumps;

static struct {
    struct date date;
    uint16_t str_size;
    char str[DATE_STR_SIZE];
    enum date_part curr_date_part;
} date_tmp;

static const uint16_t eeprom_addresses[] = {
    [NAME] = EEPROM_SIZE,
    [DELAY] = 0,
    [PUMP] = DATE_STR_SIZE
};

static uint16_t eeprom_get_plant_item_addr(size_t plant_index, enum plant_setting setting);
static bool has_saved_settings();
static void read_plant_setting(size_t plant_index, enum plant_setting setting, char* buf, size_t buf_len);
static void write_plant_setting(size_t plant_index, enum plant_setting setting, char* buf);
static void encoder_turn_right();
static void encoder_turn_left();
static void encoder_click();
static void encoder_double_click();
static void encoder_hold();
static void save_eeprom_key(size_t plants_len);
static void read_plant_settings(size_t plant_index);
static void generate_empty_plant_settings(size_t plant_index);
static void set_encoder_handlers(struct encoder* enc);
static void date_tmp_init();
static void date_tmp_update_str();
static void date_item_setting_start();
static void date_item_setting_update();
static void date_item_setting_next();
static void date_item_setting_end();
static void date_item_setting_cancel();
static void set_menu_item_setting_handlers(struct menu_item item);
static void autopumps_init(struct plant plants[], size_t plants_len);
static void autopump_start_delay(struct timer_millis* timer, struct plant* plant);

void autopump_init(struct lcd1602* lcd, struct encoder* enc, struct plant p[], size_t plants_len) {
    date_tmp_init();
    autopumps_init(p, plants_len);

    bool has_saved_set = has_saved_settings();
    if (!has_saved_set) {
        save_eeprom_key(plants_len);
    }

    menu = menu_create(
            lcd,
            coordinates_zeros(),
            coordinates_create(LCD_WIDTH - 1, LCD_HEIGHT - 1),
            plants_len
    );
    encoder = enc;
    char plant_item_key[8] = { 0 };
    for (size_t i = 0; i < plants_len; i++) {
        menu_item_list_init(menu, i, PLANT_STTNG_LEN);
        sprintf(plant_item_key, "Plant%d", (int) i + 1);
        menu_item_set(
                menu,
                i,
                NAME,
                menu_item_create(plant_item_key, autopumps.plants[i].name, LCD_WIDTH, true)
        );
        if (has_saved_set) {
            read_plant_settings(i);
        } else {
            generate_empty_plant_settings(i);
        }
        for (enum plant_setting setting = DELAY; setting < PLANT_STTNG_LEN; setting++) {
            plant_get_date_setting(autopumps.plants[i], setting, &date_tmp.date);
            date_tmp_update_str();
            struct menu_item item = menu_item_create(plant_setting_str[setting], date_tmp.str, LCD_WIDTH, false);
            set_menu_item_setting_handlers(item);
            menu_item_set(
                    menu,
                    i,
                    setting,
                    item
            );
        }
        autopump_start_delay(autopumps.pumps[i].timer, autopumps.plants + i);
    }
    set_encoder_handlers(encoder);
    menu_update_screen(menu);
}

void autopump_timers_update() {
    for (size_t i = 0; i < autopumps.len; i++) {
        timer_millis_upd(autopumps.pumps[i].timer);
    }
}

static void autopumps_init(struct plant p[], size_t plants_len) {
    autopumps.len = plants_len;
    autopumps.plants = malloc(sizeof(struct plant) * plants_len);
    memcpy(autopumps.plants, p, sizeof(struct plant) * plants_len);
    autopumps.pumps = malloc(sizeof(struct autopump) * plants_len);
    for (size_t i = 0; i < plants_len; i++) {
        autopumps.pumps[i] = (struct autopump) {
            .plant = autopumps.plants + i,
            .timer = timer_millis_create(0)
        };
        pin_mode(p[i].pump_pin, OUTPUT);
    }
}

static void save_eeprom_key(size_t plants_len) {
    eeprom_clean(EEPROM_ADDR_START, EEPROM_ADDR_START + plants_len * EEPROM_NAME_SIZE);
    eeprom_write_string(EEPROM_KEY_ADDR, EEPROM_KEY);
}

static void date_tmp_init() {
    date_tmp.str_size = DATE_STR_SIZE;
    date_tmp.curr_date_part = 0;
}

static void date_tmp_update_str() {
    date_to_string(date_tmp.date, date_tmp.str, date_tmp.str_size);
}

static void set_encoder_handlers(struct encoder* enc) {
    enc->handlers[ENC_STATE_TURN_RIGHT] = encoder_turn_right;
    enc->handlers[ENC_STATE_TURN_LEFT] = encoder_turn_left;
    enc->handlers[ENC_STATE_CLICK] = encoder_click;
    enc->handlers[ENC_STATE_DOUBLE_CLICK] = encoder_double_click;
    enc->handlers[ENC_STATE_HOLD] = encoder_hold;
}

static void set_menu_item_setting_handlers(struct menu_item item) {
    item.handlers[ITEM_STTNG_START] = date_item_setting_start;
    item.handlers[ITEM_STTNG_END] = date_item_setting_end;
    item.handlers[ITEM_STTNG_UPDATE] = date_item_setting_update;
    item.handlers[ITEM_STTNG_NEXT] = date_item_setting_next;
    item.handlers[ITEM_STTNG_CANCEL] = date_item_setting_cancel;

}

static void generate_empty_plant_settings(size_t plant_index) {
    date_tmp.date = date_empty();
    date_tmp_update_str();
    autopumps.plants[plant_index].delay = date_tmp.date;
    write_plant_setting(plant_index, DELAY, date_tmp.str);
    autopumps.plants[plant_index].pump = date_tmp.date;
    write_plant_setting(plant_index, PUMP, date_tmp.str);
}

static void read_plant_settings(size_t plant_index) {
    read_plant_setting(plant_index, DELAY, date_tmp.str, date_tmp.str_size);
    autopumps.plants[plant_index].delay = date_from_string(date_tmp.str);
    read_plant_setting(plant_index, PUMP, date_tmp.str, date_tmp.str_size);
    autopumps.plants[plant_index].pump = date_from_string(date_tmp.str);
}

static uint16_t eeprom_get_plant_item_addr(size_t plant_index, enum plant_setting setting) {
    uint16_t addr = eeprom_addresses[setting];
    if (eeprom_addresses[setting] != EEPROM_SIZE) addr += EEPROM_ADDR_START + plant_index * EEPROM_NAME_SIZE;
    return addr;
}

static bool has_saved_settings() {
    char saved_key[EEPROM_KEY_LEN];
    eeprom_read_string(EEPROM_KEY_ADDR, saved_key, EEPROM_KEY_LEN);
    return !memcmp(saved_key, EEPROM_KEY, EEPROM_KEY_LEN);
}

static void read_plant_setting(size_t plant_index, enum plant_setting setting, char* buf, size_t buf_len) {
    eeprom_read_string(
            eeprom_get_plant_item_addr(plant_index, setting),
            buf,
            buf_len
    );
}

static void write_plant_setting(size_t plant_index, enum plant_setting setting, char* buf) {
    eeprom_write_string(
            eeprom_get_plant_item_addr(plant_index, setting),
            buf
    );
    struct autopump curr_pump = autopumps.pumps[plant_index];
    autopump_start_delay(curr_pump.timer, curr_pump.plant);
}

/**
 * Encoder handler functions and functions for managing menus and plant settings.
 */

static void date_item_setting_start() {
    size_t curr_plant = menu_get_curr_list_index(menu);
    if (curr_plant >= autopumps.len) {
        menu_previous_state(menu);
        return;
    }
    enum plant_setting curr_setting = (enum plant_setting) menu_get_curr_item_index(menu);
    if (!plant_get_date_setting(autopumps.plants[curr_plant], curr_setting, &date_tmp.date)) {
        menu_previous_state(menu);
        return;
    }
    date_tmp_update_str();
    date_tmp.curr_date_part = 0;
}

static void date_item_setting_update() {
    enum encoder_state enc_state = encoder_get_curr_state(encoder);
    if (enc_state == ENC_STATE_TURN_RIGHT) {
        date_part_inc(&date_tmp.date, date_tmp.curr_date_part);
    } else if (enc_state == ENC_STATE_TURN_LEFT) {
        date_part_dec(&date_tmp.date, date_tmp.curr_date_part);
    }
    date_tmp_update_str();
    menu_set_value(menu, date_tmp.str);
}

static void date_item_setting_next() {
    if (date_tmp.curr_date_part + 1 == DATE_PART_LEN) {
        menu_item_setting_end(menu);
    } else {
        date_tmp.curr_date_part++;
    }
}

static void date_item_setting_end() {
    size_t curr_plant = menu_get_curr_list_index(menu);
    if (curr_plant >= autopumps.len) {
        menu_previous_state(menu);
        return;
    }
    enum plant_setting curr_setting = (enum plant_setting) menu_get_curr_item_index(menu);
    write_plant_setting(curr_plant, curr_setting, date_tmp.str);
    plant_set_date_setting(autopumps.plants + curr_plant, curr_setting, date_tmp.date);
}

static void date_item_setting_cancel() {
    size_t curr_plant = menu_get_curr_list_index(menu);
    if (curr_plant >= autopumps.len) {
        menu_previous_state(menu);
        return;
    }
    enum plant_setting curr_setting = (enum plant_setting) menu_get_curr_item_index(menu);
    plant_get_date_setting(autopumps.plants[curr_plant], curr_setting, &date_tmp.date);
    date_tmp_update_str();
    menu_set_value(menu, date_tmp.str);
}

static void encoder_turn_right() {
    menu_next(menu);
}

static void encoder_turn_left() {
    menu_previous(menu);
}

static void encoder_click() {
    menu_next_state(menu);
}

static void encoder_double_click() {
    menu_previous_state(menu);
}

static void encoder_hold() {
    size_t plant_i = menu_get_curr_list_index(menu);
    generate_empty_plant_settings(plant_i);
}

/**
 * Direct control of plant pumps: configuration and start of the timer.
 */

static void autopump_start_pump(struct timer_millis* timer, struct plant* plant);

static void autopump_start_delay(struct timer_millis* timer, struct plant* plant) {
    digital_write(plant->pump_pin, LOW);
    timer_millis_reconfig(timer, plant->delay);
    timer_millis_set_handler(
            timer,
            timer_millis_param_handler_create(plant, (timer_millis_handler*) autopump_start_pump)
    );
    timer_millis_start(timer);
}

static void autopump_start_pump(struct timer_millis* timer, struct plant* plant) {
    digital_write(plant->pump_pin, HIGH);
    timer_millis_reconfig(timer, plant->pump);
    timer_millis_set_handler(
            timer,
            timer_millis_param_handler_create(plant, (timer_millis_handler*) autopump_start_delay)
    );
    timer_millis_start(timer);
}
