#include "score_display.h"
#include "powerup.h"
#include "player.h"
#include "enemy.h"

#include <bn_core.h>
#include <bn_display.h>
#include <bn_keypad.h>
#include <bn_rect.h>
#include <bn_size.h>
#include <bn_string.h>
#include <bn_sprite_ptr.h>
#include <bn_sprite_text_generator.h>
#include <bn_random.h>
#include <bn_math.h>

#include "common_fixed_8x16_font.h"
#include "bn_sprite_items_cat.h"
#include "bn_sprite_items_rat.h"
#include "bn_sprite_items_cheese.h"
#include "bn_sprite_items_home.h"

// Width and height of the the player bounding box
static constexpr bn::size PLAYER_SIZE = {8, 8};
static constexpr bn::size ENEMY_SIZE = {8, 8};
static constexpr bn::size POWERUP_SIZE = {16, 16};

static constexpr int MIN_Y = -bn::display::height() / 2;
static constexpr int MAX_Y = bn::display::height() / 2;
static constexpr int MIN_X = -bn::display::width() / 2;
static constexpr int MAX_X = bn::display::width() / 2;


static constexpr int ENEMY_SPAWN_INTERVAL = 600;  // 10 seconds

// Enemy separation distance — if closer than this, push apart
static constexpr int ENEMY_SEPARATION = 14;
/**
 * The player character, controlled by the user. Can move in 4 directions and collect powerups.
 * Has a bounding box for collision detection and state for active powerups.
 */
static bn::rect create_bounding_box(bn::sprite_ptr sprite, bn::size box_size) {
    return bn::rect(sprite.x().round_integer(), sprite.y().round_integer(),
                    box_size.width(), box_size.height());
}

/**
 * Main game loop. Initializes player, score display, and enemy vector. Each frame, updates
 * player movement, checks for collisions with enemies and powerups, updates enemy positions,
 * and manages score display.
 */
int main() {
    bn::core::init();

    ScoreDisplay scoreDisplay = ScoreDisplay();
    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> screen1_sprites;

    scoreDisplay.text_generator.set_center_alignment();
    scoreDisplay.text_generator.generate(0, 0, "GET THE CHEESE!", screen1_sprites);
    scoreDisplay.text_generator.generate(0, -64, "PRESS START", screen1_sprites);

    while (bn::keypad::start_held()) {
        bn::core::update();
    }
    while(!bn::keypad::start_pressed()) {
        bn::core::update();
    }
    screen1_sprites.clear();

    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> screen2_sprites;
    scoreDisplay.text_generator.set_center_alignment();
    scoreDisplay.text_generator.generate(0, -16, "USE ARROWS TO MOVE", screen2_sprites);
    scoreDisplay.text_generator.generate(0, -32, "RETURN HOME TO SAFE", screen2_sprites);
    scoreDisplay.text_generator.generate(0, -48, "AVOID THE CATS!", screen2_sprites);
    scoreDisplay.text_generator.generate(0, 0, "PRESS START!!!", screen2_sprites);

    while (bn::keypad::start_held()) {
        bn::core::update();
    }
    while(!bn::keypad::start_pressed()) {
        bn::core::update();
    }
    screen2_sprites.clear();

    /*
     * Powerup management
     * random number generator for powerup spawn positions and types
     * at most 2 active powerups at once
     */
    bn::vector<Powerup, 2> powerups;
    int powerup_spawn_timer = POWERUP_SPAWN_INTERVAL;
    bn::random rng;

    bn::sprite_ptr home_sprite = bn::sprite_items::home.create_sprite(80, 0);

    Player player = Player(80, 0, 3.5, PLAYER_SIZE);

    // Create a vector of enemies with different starting positions and speeds.
    // Later enemies are faster than earlier ones.
    bn::vector<Enemy, 4> enemies;  // 4 enemies max
    enemies.push_back(Enemy(25, 21, bn::fixed(.75), ENEMY_SIZE));

    int enemy_spawn_timer = ENEMY_SPAWN_INTERVAL;  // Timer for spawning new enemies
    int enemy_jump_timer = 360;
    bn::fixed enemy_speed[4] = {bn::fixed(.75), bn::fixed(1.0), bn::fixed(1.5), bn::fixed(1.75)};

    while(true) {
        player.update();

        bn::rect home_box = create_bounding_box(home_sprite, bn::size(16, 16));
        bool in_home = home_box.intersects(player.bounding_box);

        if (in_home) {
            player.is_invincible = true;
        } else if(!player.has_powerup) {
            player.is_invincible = false;
        }

        // Periodically teleport first enemy to a random position
        enemy_jump_timer--;
        if(enemy_jump_timer <= 0) {
            enemy_jump_timer = 360;
            int jx = rng.get_int(MIN_X + 16, MAX_X - 16);
            int jy = rng.get_int(MIN_Y + 16, MAX_Y - 16);
            enemies[0].enemy_sprite.set_x(jx);
            enemies[0].enemy_sprite.set_y(jy);
        }

        // Spawn new enemies over time
        enemy_spawn_timer--;
        if(enemy_spawn_timer <= 0 && enemies.size() < enemies.max_size()) {
            enemy_spawn_timer = ENEMY_SPAWN_INTERVAL;
            int ex = rng.get_int(MIN_X + 16, MAX_X - 16);
            int ey = rng.get_int(MIN_Y + 16, MAX_Y - 16);
            // BUG FIX: use enemies.size() (not enemies.size() - 1) to get the next speed tier
            enemies.push_back(Enemy(ex, ey, enemy_speed[enemies.size()], ENEMY_SIZE));
        }

        // Update enemies and check for collisions with player
        for(Enemy& enemy : enemies) {
            if(!in_home) {
                enemy.update(player);
            }

            if(!player.is_invincible && enemy.bounding_box.intersects(player.bounding_box)) {
                scoreDisplay.resetScore();
                // BUG FIX: reset player to home position (80, 0) instead of (44, 22)
                player.sprite.set_x(80);
                player.sprite.set_y(0);

                while(enemies.size() > 1) {
                    enemies.pop_back();
                }
                enemy_spawn_timer = ENEMY_SPAWN_INTERVAL;  // Reset enemy spawn timer on player death
                break;
            }
        }

        // Separate enemies from each other (no overlapping)
        for(int i = 0; i < enemies.size(); i++) {
            for(int j = i + 1; j < enemies.size(); j++) {
                enemies[i].separateFrom(enemies[j]);
            }
        }

        // Spawn a new powerup randomly
        powerup_spawn_timer--;
        if(powerup_spawn_timer <= 0 && powerups.size() < powerups.max_size()) {
            powerup_spawn_timer = POWERUP_SPAWN_INTERVAL;
            int px = rng.get_int(MIN_X + 16, MAX_X - 16);
            int py = rng.get_int(MIN_Y + 16, MAX_Y - 16);
            // randomly choose between speed boost and invincibility
            PowerupType type = (rng.get_int(2) == 0) ? PowerupType::SPEED_BOOST : PowerupType::INVINCIBILITY;
            powerups.push_back(Powerup(px, py, type, POWERUP_SIZE));
        }

        for(int i = powerups.size() - 1; i >= 0; i--) {
            if(!powerups[i].active) continue;
            powerups[i].bounding_box = create_bounding_box(powerups[i].sprite, powerups[i].powerup_size);

            if(powerups[i].bounding_box.intersects(player.bounding_box)) {
                player.applyPowerup(powerups[i].powerup_type);
                powerups[i].deactivate();
                powerups.erase(powerups.begin() + i);
            }
        }

        // Update the scores and display them
        if(!in_home) {
            scoreDisplay.update();
        }

        bn::core::update();
    }
}