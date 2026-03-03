#ifndef POWERUP_H
#define POWERUP_H

#include <bn_sprite_ptr.h>
#include <bn_size.h>
#include <bn_rect.h>

// Powerup duration in frames (60 frames = ~1 second)
static constexpr int POWERUP_DURATION = 300;  // 5 seconds

// How often a powerup spawns (in frames)
static constexpr int POWERUP_SPAWN_INTERVAL = 600;  // 10 seconds

/**
 * Types of powerups available in the game. SPEED_BOOST doubles player speed AND
 * INVINCIBILITY makes player immune to enemies.
 */
enum class PowerupType {
    SPEED_BOOST,
    INVINCIBILITY
};

/**
 * A collectible powerup that temporarily grants the player either a speed boost or invincibility.
 * Spawns at a fixed position and is removed when collected by the player.
 */
class Powerup {
public:
    Powerup(int x, int y, PowerupType type, bn::size size);

    void deactivate();

    bn::sprite_ptr sprite;
    bool active;               // Whether this powerup is still collectible
    PowerupType powerup_type;  // Which effect this powerup grants
    bn::size powerup_size;     // The width and height of the sprite
    bn::rect bounding_box;     // The rectangle around the sprite for checking collision
};

#endif