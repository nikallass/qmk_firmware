/* Copyright 2024 @ Keychron (https://www.keychron.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "keychron_common.h"

enum layers{
  MAC_BASE,
  WIN_BASE,
  MAC_FN1,
  WIN_FN1,
  FN2
};

// Enum for tap dance state - defines special key combination for ESC/grave
enum {
    TD_ESC_GRV = 0
};

// Structure to track the state of tap dance
typedef struct {
    bool is_double_tapped;    // Tracks if key was double-tapped
    uint16_t timer;           // Timer for tracking tap timeout
} td_tap_t;

// Global state for tap dance
static td_tap_t td_state = {
    .is_double_tapped = false,
    .timer = 0
};

// Timeout period for tap dance (in milliseconds)
#define TAP_TIMEOUT 500

// Handles what happens when tap dance is completed
void esc_grave_finished(tap_dance_state_t *state, void *user_data) {
    if (state->count >= 2) {  // On double-tap or more
        td_state.is_double_tapped = true;
        td_state.timer = timer_read();
        // Send two backticks on initial activation
        tap_code(KC_GRV);
        tap_code(KC_GRV);
    } else if (state->count == 1 &&  // On single tap
               (!td_state.is_double_tapped || timer_elapsed(td_state.timer) >= TAP_TIMEOUT)) {
        tap_code(KC_ESC);
    }
}

// Resets tap dance state when timeout is reached
void esc_grave_reset(tap_dance_state_t *state, void *user_data) {
    if (timer_elapsed(td_state.timer) >= TAP_TIMEOUT) {
        td_state.is_double_tapped = false;
    }
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_ESC_GRV] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, esc_grave_finished, esc_grave_reset)
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Get current state of all modifier keys (Shift, Ctrl, Alt, GUI/CMD)
    uint8_t mods = get_mods();

    if (keycode == TD(TD_ESC_GRV)) {
        // Handle CMD+SHIFT+ESC combination
        // When pressed, outputs CMD+SHIFT+grave (tilda)
        if ((mods & MOD_MASK_GUI) && (mods & MOD_MASK_SHIFT)) {
            if (record->event.pressed) {
                tap_code16(G(S(KC_GRV)));
            }
            return false;
        }

        // Handle SHIFT+ESC combination
        // When pressed, outputs grave (`)
        if ((mods & MOD_MASK_SHIFT) && !(mods & ~MOD_MASK_SHIFT)) {
            if (record->event.pressed) {
                unregister_mods(MOD_MASK_SHIFT);
                tap_code(KC_GRV);
                register_mods(mods);
            }
            return false;
        }

        // Handle ALT+ESC combination
        // When pressed, outputs SHIFT+grave (tilda)
        if ((mods & MOD_MASK_ALT) && !(mods & ~MOD_MASK_ALT)) {
            if (record->event.pressed) {
                unregister_mods(MOD_MASK_ALT);
                tap_code16(S(KC_GRV));
                register_mods(mods);
            }
            return false;
        }

        // Handle CMD/GUI+ESC combination
        // When pressed, outputs CMD+grave
        if ((mods & MOD_MASK_GUI) && !(mods & ~MOD_MASK_GUI)) {
            if (record->event.pressed) {
                tap_code16(G(KC_GRV));
            }
            return false;
        }

        // Handle double-tap backtick mode
        // If ESC was double-tapped earlier and we're still within the timeout period,
        // continue outputting backticks for each subsequent press
        if (!mods && td_state.is_double_tapped &&
            timer_elapsed(td_state.timer) < TAP_TIMEOUT &&
            record->event.pressed) {
            tap_code(KC_GRV);
            td_state.timer = timer_read();
            return false;
        }
    }

    // Process Keychron-specific keyboard functions
    if (!process_record_keychron_common(keycode, record)) {
        return false;
    }

    // If none of the above conditions are met, process the key normally
    return true;
}

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[MAC_BASE] = LAYOUT_ansi_68(
     TD(TD_ESC_GRV),   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,           KC_DEL,
     KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,           KC_HOME,
     KC_F17,   KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,            KC_PGUP,
     KC_LSFT,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,                      KC_RSFT,  KC_UP,   KC_PGDN,
     KC_LCTL,  KC_LOPTN, KC_LCMMD,                               KC_SPC,                                 KC_RCMMD,MO(MAC_FN1),MO(FN2), KC_LEFT,  KC_DOWN, KC_RGHT),

[WIN_BASE] = LAYOUT_ansi_68(
     TD(TD_ESC_GRV),   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,           KC_DEL,
     KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,           KC_HOME,
     KC_F17,   KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,            KC_PGUP,
     KC_LSFT,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,                      KC_RSFT,  KC_UP,   KC_PGDN,
     KC_LCTL,  KC_LGUI,  KC_LALT,                                KC_SPC,                                 KC_RALT, MO(WIN_FN1),MO(FN2), KC_LEFT,  KC_DOWN, KC_RGHT),

[MAC_FN1] = LAYOUT_ansi_68(
     KC_GRV,   KC_BRID,  KC_BRIU,  KC_MCTRL, KC_LNPAD, RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  _______,            _______,
     _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            KC_END,
     RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,   KC_GRV,            _______,            _______,
     _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  NK_TOGG,  _______,  _______,  _______,  _______,                      _______,  _______,  _______,
     _______,  _______,  _______,                                ROPT(KC_SPC),                                _______,  _______,  _______,  _______,  _______,  _______),

[WIN_FN1] = LAYOUT_ansi_68(
     KC_GRV,   KC_BRID,  KC_BRIU,  KC_TASK,  KC_FILE,  RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  _______,            _______,
     _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            KC_END,
     RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,   KC_GRV,            _______,            _______,
     _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  NK_TOGG,  _______,  _______,  _______,  _______,                      _______,  _______,  _______,
     _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______),

[FN2] = LAYOUT_ansi_68(
      KC_GRV,     KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   _______,            _______,
     _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
     _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   KC_GRV,            _______,            _______,
     _______,  _______,  _______,  _______,  _______,  BAT_LVL,  _______,  _______,  _______,  _______,  _______,                      _______,  _______,  _______,
     _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______),
};

