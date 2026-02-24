#include <bn_core.h>
#include <bn_backdrop.h>
#include <bn_color.h>
#include <bn_keypad.h>

// Width and height of the the player bounding box
static constexpr bn::size PLAYER_SIZE = {8, 8};

static constexpr int MIN_Y = -bn::display::height() / 2;
static constexpr int MAX_Y = bn::display::height() / 2;
static constexpr int MIN_X = -bn::display::width() / 2;
static constexpr int MAX_X = bn::display::width() / 2;

// Number of characters required to show the longest numer possible in an int (-2147483647)
static constexpr int MAX_SCORE_CHARS = 11;

// Score location
static constexpr int SCORE_X = 70;
static constexpr int SCORE_Y = -70;

// High score location
static constexpr int HIGH_SCORE_X = -70;
static constexpr int HIGH_SCORE_Y = -70;

int main() {
    bn::core::init();

    bn::backdrop::set_color(bn::color(31, 0, 0));

    while(true) {
        if(bn::keypad::a_pressed()) {
            bn::backdrop::set_color(bn::color(0, 0, 30));
        }
        if(bn::keypad::b_pressed()) {
            bn::backdrop::set_color(bn::color(0, 31, 0));
        }
        bn::core::update();
    }
}