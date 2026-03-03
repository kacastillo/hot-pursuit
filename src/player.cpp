#include "player.h"
#include "utils.h"

#include <bn_keypad.h>
#include <bn_display.h>
#include <bn_sprite_items_rat.h>

static constexpr int MIN_Y = -bn::display::height() / 2;
static constexpr int MAX_Y = bn::display::height() / 2;
static constexpr int MIN_X = -bn::display::width() / 2;
static constexpr int MAX_X = bn::display::width() / 2;

static constexpr int POWERUP_DURATION = 300;

Player::Player(int starting_x, int starting_y,
               bn::fixed player_speed, bn::size player_size) :
    sprite(bn::sprite_items::rat.create_sprite(starting_x, starting_y)),
    base_speed(player_speed),
    speed(player_speed),
    size(player_size),
    bounding_box(create_bounding_box(sprite, size)),
    powerup_timer(0),
    has_powerup(false),
    is_invincible(false)
{
}

void Player::update() {

    // Handle active powerup countdown
    if(has_powerup) {
        powerup_timer--;
        if(powerup_timer <= 0) {
            has_powerup = false;
            is_invincible = false;
            speed = base_speed; 
        }
    }

    // Movement
    if(bn::keypad::right_held()) {
        sprite.set_x(sprite.x() + speed);
    }
    if(bn::keypad::left_held()) {
        sprite.set_x(sprite.x() - speed);
    }
    if(bn::keypad::down_held()) {
        sprite.set_y(sprite.y() + speed);
    }
    if(bn::keypad::up_held()) {
        sprite.set_y(sprite.y() - speed);
    }

    // Prevent leaving screen (X)
    if(sprite.x() < MIN_X + size.width() / 2) {
        sprite.set_x(MIN_X + size.width() / 2);
    }
    if(sprite.x() > MAX_X - size.width() / 2) {
        sprite.set_x(MAX_X - size.width() / 2);
    }

    // Prevent leaving screen (Y)
    if(sprite.y() < MIN_Y + size.height() / 2) {
        sprite.set_y(MIN_Y + size.height() / 2);
    }
    if(sprite.y() > MAX_Y - size.height() / 2) {
        sprite.set_y(MAX_Y - size.height() / 2);
    }

    bounding_box = create_bounding_box(sprite, size);
}

void Player::applyPowerup(PowerupType type) {

    has_powerup = true;
    powerup_timer = POWERUP_DURATION;

    if(type == PowerupType::SPEED_BOOST) {
        speed = base_speed * 2;
        is_invincible = false;
    }
    else if(type == PowerupType::INVINCIBILITY) {
        is_invincible = true;
        speed = base_speed;
    }
}