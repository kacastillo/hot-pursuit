#ifndef ENEMY_H
#define ENEMY_H

#include <bn_sprite_ptr.h>
#include <bn_rect.h>
#include <bn_size.h>
#include <bn_fixed.h>
#include <bn_random.h>

#include "player.h"


class Enemy {
public:
    Enemy(int starting_x, int starting_y,
          bn::fixed enemy_speed, bn::size enemy_size);

    void update(Player& player);
    void separateFrom(Enemy& other);

    bn::sprite_ptr enemy_sprite;
    bn::rect bounding_box;

private:
    bn::fixed speed;
    bn::size size;
    bn::random rng;
};

#endif