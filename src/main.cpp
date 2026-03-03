#include "bn_core.h"
#include "bn_display.h"
#include "bn_keypad.h"
#include "bn_random.h"
#include "bn_vector.h"

#include "bn_sprite_items_home.h"

#include "ScoreDisplay.h"
#include "Player.h"
#include "Enemy.h"
#include "Powerup.h"
#include "PowerupType.h"

// Width and height of the player, enemy, and powerup bounding boxes
static constexpr bn::size PLAYER_SIZE{8, 8};
static constexpr bn::size ENEMY_SIZE{8, 8};
static constexpr bn::size POWERUP_SIZE{16, 16};

// Screen bounds
static constexpr int MIN_Y = -bn::display::height() / 2;
static constexpr int MAX_Y = bn::display::height() / 2;
static constexpr int MIN_X = -bn::display::width() / 2;
static constexpr int MAX_X = bn::display::width() / 2;

// Powerup duration in frames (60 frames = ~1 second)
static constexpr int POWERUP_DURATION = 300;        // 5 seconds

// How often a powerup spawns (in frames)
static constexpr int POWERUP_SPAWN_INTERVAL = 600;  // 10 seconds
static constexpr int ENEMY_SPAWN_INTERVAL = 600;    // 10 seconds

/**
 * Helper function to create a bounding box around a sprite.
 */
bn::rect create_bounding_box(const bn::sprite_ptr& sprite, bn::size box_size) {
    return bn::rect(sprite.x().round_integer(),
                    sprite.y().round_integer(),
                    box_size.width(),
                    box_size.height());
}

/**
 * Main game loop. Initializes player, score display, enemies, and powerups.
 * Each frame updates player movement, enemies, powerups, collision detection,
 * and score display.
 */
int main() {
    bn::core::init();

    // Initialize score display
    ScoreDisplay scoreDisplay;

    // Display title screen
    bn::vector<bn::sprite_ptr, 8> title_sprites;
    scoreDisplay.text_generator.generate(0, 0, "GET THE CHEESE!", title_sprites);
    scoreDisplay.text_generator.generate(0, -16, "PRESS START", title_sprites);

    // Wait for player to press start
    while(!bn::keypad::start_pressed()) {
        bn::core::update();
    }
    title_sprites.clear();

    // Initialize player at starting position
    Player player(80, 0, 3.5, PLAYER_SIZE);

    // Create a home sprite for player to return to
    bn::sprite_ptr home_sprite = bn::sprite_items::home.create_sprite(80, 0);

    // Create vector of enemies with different starting positions and speeds
    bn::vector<Enemy, 4> enemies;
    enemies.push_back(Enemy(25, 21, 0.75, ENEMY_SIZE));
    bn::fixed enemy_speeds[4] = {0.75, 1.0, 1.5, 1.75};
    int enemy_spawn_timer = ENEMY_SPAWN_INTERVAL;
    int enemy_jump_timer = 360;

    // Powerup management
    bn::vector<Powerup, 2> powerups; // At most 2 active powerups at once
    int powerup_spawn_timer = POWERUP_SPAWN_INTERVAL;
    bn::random rng;

    // Main game loop
    while(true) {
        // Update player position and bounding box
        player.update();

        // Check if player is in the home area
        bn::rect home_box = create_bounding_box(home_sprite, bn::size(16,16));
        bool in_home = home_box.intersects(player.bounding_box);

        // Make player invincible if in home or under powerup effect
        player.is_invincible = in_home || player.has_powerup;

        // Update enemies
        enemy_jump_timer--;
        if(enemy_jump_timer <= 0) {
            enemy_jump_timer = 360;
            int jx = rng.get_int(MIN_X + 16, MAX_X - 16);
            int jy = rng.get_int(MIN_Y + 16, MAX_Y - 16);
            enemies[0].enemy_sprite.set_x(jx);
            enemies[0].enemy_sprite.set_y(jy);
        }

        // Spawn new enemies at intervals
        enemy_spawn_timer--;
        if(enemy_spawn_timer <= 0 && enemies.size() < enemies.max_size()) {
            enemy_spawn_timer = ENEMY_SPAWN_INTERVAL;
            int ex = rng.get_int(MIN_X + 16, MAX_X - 16);
            int ey = rng.get_int(MIN_Y + 16, MAX_Y - 16);
            enemies.push_back(Enemy(ex, ey, enemy_speeds[enemies.size()-1], ENEMY_SIZE));
        }

        // Update each enemy and check for collisions with player
        for(Enemy& enemy : enemies) {
            if(!in_home) enemy.update(player);

            if(!player.is_invincible && enemy.bounding_box.intersects(player.bounding_box)) {
                scoreDisplay.resetScore();
                player.reset_position(44, 22); // Reset player on collision

                // Remove extra enemies
                while(enemies.size() > 1) enemies.pop_back();

                // Reset enemy spawn timer
                enemy_spawn_timer = ENEMY_SPAWN_INTERVAL;
                break;
            }
        }

        // Separate enemies to prevent overlapping
        for(int i=0; i<enemies.size(); i++) {
            for(int j=i+1; j<enemies.size(); j++) {
                enemies[i].separateFrom(enemies[j]);
            }
        }

        // Spawn new powerup randomly
        powerup_spawn_timer--;
        if(powerup_spawn_timer <= 0 && powerups.size() < powerups.max_size()) {
            powerup_spawn_timer = POWERUP_SPAWN_INTERVAL;
            int px = rng.get_int(MIN_X + 16, MAX_X - 16);
            int py = rng.get_int(MIN_Y + 16, MAX_Y - 16);

            // Randomly choose between speed boost and invincibility
            PowerupType type = (rng.get_int(2) == 0) ? PowerupType::SPEED_BOOST : PowerupType::INVINCIBILITY;
            powerups.push_back(Powerup(px, py, type, POWERUP_SIZE));
        }

        // Update powerups and check if collected by player
        for(int i = powerups.size() - 1; i >= 0; i--) {
            if(!powerups[i].active) continue;
            powerups[i].update_bounding_box();
            if(powerups[i].bounding_box.intersects(player.bounding_box)) {
                player.applyPowerup(powerups[i].powerup_type);
                powerups[i].deactivate();
                powerups.erase(powerups.begin() + i);
            }
        }

        // Update the score display (not while in home)
        if(!in_home) scoreDisplay.update();

        bn::core::update();
    }
}