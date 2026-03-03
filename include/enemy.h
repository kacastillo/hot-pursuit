#ifndef ENEMY_H
#define ENEMY_H

#include <bn_sprite_ptr.h>
#include <bn_fixed.h>
#include <bn_size.h>
#include <bn_rect.h>
#include <bn_random.h>

#include "player.h"

/**
 * An enemy that chases the player. If it catches the player, the player is reset.
 * Multiple enemies spread apart to avoid overlapping.
 */
class Enemy {
public:
    Enemy(int starting_x, int starting_y, bn::fixed enemy_speed, bn::size enemy_size);

    /** moves enemy towards the player. */
    void update(Player& player);

    /**
     * Push this enemy away from another enemy if they are overlapping.
     * Computes a repulsion vector based on the distance between the two enemies
     * and moves both apart by half the overlap, keeping enemies spread out.
     */
    void separateFrom(Enemy& other);

    bn::sprite_ptr enemy_sprite;
    bn::fixed speed;        // The speed of the enemy
    bn::size size;           // The width and height of the enemy sprite
    bn::rect bounding_box;   // The rectangle around the enemy sprite for checking collision
    bn::random rng;          // random number generator
};

#endif