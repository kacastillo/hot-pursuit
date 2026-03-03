#ifndef PLAYER_H
#define PLAYER_H

#include <bn_sprite_ptr.h>
#include <bn_fixed.h>
#include <bn_size.h>
#include <bn_rect.h>

#include "powerup.h"

/**
 * The player character, controlled by the user. Can move in 4 directions and collect powerups.
 * Has a bounding box for collision detection and state for active powerups.
 */
class Player {
public:
    Player(int starting_x, int starting_y, bn::fixed player_speed, bn::size player_size);

    /**
     * Update the position and bounding box of the player based on d-pad movement.
     * Also prevents the player from moving off the screen.
     */
    void update();

    /**
     * Apply a powerup effect to the player.
     */
    void applyPowerup(PowerupType type);

    bn::sprite_ptr sprite;
    bn::fixed base_speed;    // The normal speed of the player (before any powerup)
    bn::fixed speed;         // The current speed of the player
    bn::size size;           // The width and height of the sprite
    bn::rect bounding_box;   // The rectangle around the sprite for checking collision
    int powerup_timer;       // Frames remaining on the active powerup
    bool has_powerup;        // Whether a powerup is currently active
    bool is_invincible;      // Whether the player is currently invincible
};

#endif