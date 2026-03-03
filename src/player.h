#ifndef PLAYER_H
#define PLAYER_H

#include <bn_sprite_ptr.h>
#include <bn_rect.h>
#include <bn_size.h>
#include <bn_fixed.h>

#include "PowerupType.h"

class Player {
public:
    Player(int starting_x, int starting_y,
           bn::fixed player_speed, bn::size player_size);

    void update();
    void applyPowerup(PowerupType type);

    bn::sprite_ptr sprite;
    bn::fixed base_speed;
    bn::fixed speed;
    bn::size size;
    bn::rect bounding_box;

    int powerup_timer;
    bool has_powerup;
    bool is_invincible;
};

#endif