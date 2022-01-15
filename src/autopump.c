#include "autopump.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "lcd1602.h"
#include "coordinates.h"
#include "menu.h"
#include "encoder.h"
#include "time.h"
#include "eeprom.h"
#include "plant.h"
#include "timer_millis.h"
#include "gpio.h"

#define EEPROM_KEY_ADDR 0
#define EEPROM_KEY "Autopump"
#define EEPROM_KEY_LEN 9
#define EEPROM_ADDR_START EEPROM_KEY_LEN
#define EEPROM_NAME_SIZE 18
#define DATE_STR_SIZE 9

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
    struct time time;
    uint16_t str_size;
    char str[DATE_STR_SIZE];
    enum time_part curr_time_part;
} time_tmp;

static struct menu* menu;
static struct encoder* encoder;
static const uint16_t eeprom_addresses[] = {
    [NAME] = EEPROM_SIZE,
    [DELAY] = 0,
    [PUMP] = DATE_STR_SIZE
};

static bool has_saved_settings();
static void save_eeprom_key(size_t plants_len);
static void read_plant_settings(size_t plant_index);
static void generate_empty_plant_settings(size_t plant_index);

static void set_menu_item_setting_handlers(struct menu_item item);
static void set_encoder_handlers(struct encoder* enc);

static void time_tmp_init();
static void time_tmp_update_str();

static void autopumps_init(const struct plant plants[], size_t plants_len);
static void autopump_start_delay(struct timer_millis* timer, struct plant* plant);

void autopump_init(struct lcd1602* const lcd, struct encoder* const enc, const struct plant p[], const size_t plants_len) {
    time_tmp_init();
    autopumps_init(p, plants_len);

    const bool has_saved_set = has_saved_settings();
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
            plant_get_time_setting(autopumps.plants[i], setting, &time_tmp.time);
            time_tmp_update_str();
            const struct menu_item item = menu_item_create(plant_setting_to_str(setting), time_tmp.str, LCD_WIDTH, false);
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

static void autopumps_init(const struct plant p[], const size_t plants_len) {
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

static void save_eeprom_key(const size_t plants_len) {
    eeprom_clean(EEPROM_ADDR_START, EEPROM_ADDR_START + plants_len * EEPROM_NAME_SIZE);
    eeprom_write_string(EEPROM_KEY_ADDR, EEPROM_KEY);
}

static void time_tmp_init() {
    time_tmp.str_size = DATE_STR_SIZE;
    time_tmp.curr_time_part = 0;
}

static void time_tmp_update_str() {
    time_to_string(time_tmp.time, time_tmp.str, time_tmp.str_size);
}

/*
 * Read and write menu settings with eeprom
 */

static bool has_saved_settings() {
    char saved_key[EEPROM_KEY_LEN];
    eeprom_read_string(EEPROM_KEY_ADDR, saved_key, EEPROM_KEY_LEN);
    return !memcmp(saved_key, EEPROM_KEY, EEPROM_KEY_LEN);
}

static uint16_t eeprom_get_plant_item_addr(const size_t plant_index, const enum plant_setting setting) {
    uint16_t addr = eeprom_addresses[setting];
    if (eeprom_addresses[setting] != EEPROM_SIZE) addr += EEPROM_ADDR_START + plant_index * EEPROM_NAME_SIZE;
    return addr;
}

static void write_plant_setting(const size_t plant_index, const enum plant_setting setting, const char* buf) {
    eeprom_write_string(
            eeprom_get_plant_item_addr(plant_index, setting),
            buf
    );
    const struct autopump curr_pump = autopumps.pumps[plant_index];
    autopump_start_delay(curr_pump.timer, curr_pump.plant);
}

static void generate_empty_plant_settings(const size_t plant_index) {
    time_tmp.time = time_empty();
    time_tmp_update_str();
    autopumps.plants[plant_index].delay = time_tmp.time;
    write_plant_setting(plant_index, DELAY, time_tmp.str);
    autopumps.plants[plant_index].pump = time_tmp.time;
    write_plant_setting(plant_index, PUMP, time_tmp.str);
}

static void read_plant_setting(const size_t plant_index, const enum plant_setting setting, char* buf, size_t buf_len) {
    eeprom_read_string(
            eeprom_get_plant_item_addr(plant_index, setting),
            buf,
            buf_len
    );
}

static void read_plant_settings(const size_t plant_index) {
    read_plant_setting(plant_index, DELAY, time_tmp.str, time_tmp.str_size);
    autopumps.plants[plant_index].delay = time_from_string(time_tmp.str);
    read_plant_setting(plant_index, PUMP, time_tmp.str, time_tmp.str_size);
    autopumps.plants[plant_index].pump = time_from_string(time_tmp.str);
}

/*
 * Encoder handler functions and functions for managing menus and plant settings.
 */

static void time_item_setting_start() {
    const size_t curr_plant = menu_get_curr_list_index(menu);
    if (curr_plant >= autopumps.len) {
        menu_previous_state(menu);
        return;
    }
    const enum plant_setting curr_setting = (enum plant_setting) menu_get_curr_item_index(menu);
    if (!plant_get_time_setting(autopumps.plants[curr_plant], curr_setting, &time_tmp.time)) {
        menu_previous_state(menu);
        return;
    }
    time_tmp_update_str();
    time_tmp.curr_time_part = 0;
}

static void time_item_setting_update() {
    const enum encoder_state enc_state = encoder_get_curr_state(encoder);
    if (enc_state == ENC_STATE_TURN_RIGHT) {
        time_part_inc(&time_tmp.time, time_tmp.curr_time_part);
    } else if (enc_state == ENC_STATE_TURN_LEFT) {
        time_part_dec(&time_tmp.time, time_tmp.curr_time_part);
    }
    time_tmp_update_str();
    menu_set_value(menu, time_tmp.str);
}

static void time_item_setting_next() {
    if (time_tmp.curr_time_part + 1 == TIME_PART_LEN) {
        menu_item_setting_end(menu);
    } else {
        time_tmp.curr_time_part++;
    }
}

static void time_item_setting_end() {
    const size_t curr_plant = menu_get_curr_list_index(menu);
    if (curr_plant >= autopumps.len) {
        menu_previous_state(menu);
        return;
    }
    const enum plant_setting curr_setting = (enum plant_setting) menu_get_curr_item_index(menu);
    write_plant_setting(curr_plant, curr_setting, time_tmp.str);
    plant_set_time_setting(autopumps.plants + curr_plant, curr_setting, time_tmp.time);
}

static void time_item_setting_cancel() {
    const size_t curr_plant = menu_get_curr_list_index(menu);
    if (curr_plant >= autopumps.len) {
        menu_previous_state(menu);
        return;
    }
    const enum plant_setting curr_setting = (enum plant_setting) menu_get_curr_item_index(menu);
    plant_get_time_setting(autopumps.plants[curr_plant], curr_setting, &time_tmp.time);
    time_tmp_update_str();
    menu_set_value(menu, time_tmp.str);
}

static void set_menu_item_setting_handlers(struct menu_item item) {
    item.handlers[ITEM_STTNG_START] = time_item_setting_start;
    item.handlers[ITEM_STTNG_END] = time_item_setting_end;
    item.handlers[ITEM_STTNG_UPDATE] = time_item_setting_update;
    item.handlers[ITEM_STTNG_NEXT] = time_item_setting_next;
    item.handlers[ITEM_STTNG_CANCEL] = time_item_setting_cancel;
}

// Encoder's handlers

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
    const size_t plant_i = menu_get_curr_list_index(menu);
    generate_empty_plant_settings(plant_i);
}

static void set_encoder_handlers(struct encoder* const enc) {
    encoder_set_handler(enc, ENC_STATE_CLICK, encoder_click);
    encoder_set_handler(enc, ENC_STATE_TURN_RIGHT, encoder_turn_right);
    encoder_set_handler(enc, ENC_STATE_TURN_LEFT, encoder_turn_left);
    encoder_set_handler(enc, ENC_STATE_DOUBLE_CLICK, encoder_double_click);
    encoder_set_handler(enc, ENC_STATE_HOLD, encoder_hold);
}

/*
 * Direct control of plant pumps: configuration and start of the timer.
 */

static void autopump_start_pump(struct timer_millis* timer, struct plant* plant);

static void autopump_start_delay(struct timer_millis* const timer, struct plant* const plant) {
    digital_write(plant->pump_pin, LOW);
    timer_millis_reconfig(timer, plant->delay);
    timer_millis_set_handler(
            timer,
            timer_millis_param_handler_create(plant, (timer_millis_handler*) autopump_start_pump)
    );
    timer_millis_start(timer);
}

static void autopump_start_pump(struct timer_millis* const timer, struct plant* const plant) {
    digital_write(plant->pump_pin, HIGH);
    timer_millis_reconfig(timer, plant->pump);
    timer_millis_set_handler(
            timer,
            timer_millis_param_handler_create(plant, (timer_millis_handler*) autopump_start_delay)
    );
    timer_millis_start(timer);
}
