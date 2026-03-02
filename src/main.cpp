#include <bn_core.h>
#include <bn_display.h>
#include <bn_keypad.h>
#include <bn_rect.h>
#include <bn_size.h>
#include <bn_string.h>
#include <bn_sprite_ptr.h>
#include <bn_sprite_text_generator.h>
#include <bn_random.h>
#include <bn_sound_items.h>
#include <bn_music_items.h>
#include <bn_math.h>

#include "common_fixed_8x16_font.h"
#include "bn_sprite_items_cat.h"
#include "bn_sprite_items_rat.h"

#include "bn_sprite_items_square.h"

// Width and height of the the player bounding box
static constexpr bn::size PLAYER_SIZE = {8, 8};
static constexpr bn::size ENEMY_SIZE = {8, 8};
static constexpr bn::size POWERUP_SIZE = {8, 8};

static constexpr int MIN_Y = -bn::display::height() / 2;
static constexpr int MAX_Y = bn::display::height() / 2;
static constexpr int MIN_X = -bn::display::width() / 2;
static constexpr int MAX_X = bn::display::width() / 2;

// Number of characters required to show two of the longest numer possible in an int (-2147483647)
static constexpr int MAX_SCORE_CHARS = 22;

// Score location
static constexpr int SCORE_X = 70;
static constexpr int SCORE_Y = -70;

// High score location
static constexpr int HIGH_SCORE_X = -70;
static constexpr int HIGH_SCORE_Y = -70;

// Powerup duration in frames (60 frames = ~1 second)
static constexpr int POWERUP_DURATION = 300; // 5 seconds
// How often a powerup spawns (in frames)
static constexpr int POWERUP_SPAWN_INTERVAL = 600; // 10 seconds
// Enemy separation distance — if closer than this, push apart
static constexpr int ENEMY_SEPARATION = 14;

/**
 * Creates a rectangle centered at a sprite's location with a given size.
 * sprite the sprite to center the box around
 * box_size the dimensions of the bounding box
 */
bn::rect create_bounding_box(bn::sprite_ptr sprite, bn::size box_size) {
    return bn::rect(sprite.x().round_integer(),
                    sprite.y().round_integer(),
                    box_size.width(),
                    box_size.height());
}

/**
 * Displays a score and high score.
 * 
 * Score starts a 0 and is increased each time update is called, and reset to 0 when resetScore is
 * called. High score tracks the highest score ever reached.
*/
class ScoreDisplay {
    public:
        ScoreDisplay() :
            score(0), // Start score at 0
            high_score(0), // Start high score at 0
            score_sprites(bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS>()), // Start with empty vector for score sprites
            text_generator(bn::sprite_text_generator(common::fixed_8x16_sprite_font)) // Use a new text generator
        {

        }

        /**
         * Increases score by 1 and updates high score if needed. Displays score and high score.
         */
        void update() {
            // increase score and update high score if this is the new highest
            score++;
            if(score > high_score) {
                high_score = score;
            }

            // Stop displaying previous scores
            score_sprites.clear();

            // Display new scores
            show_number(SCORE_X, SCORE_Y, score);
            show_number(HIGH_SCORE_X, HIGH_SCORE_Y, high_score);
        }

        /**
         * Displays a number at the given position
         */
        void show_number(int x, int y, int number) {
            // Convert number to a string and then display it
            bn::string<MAX_SCORE_CHARS> number_string = bn::to_string<MAX_SCORE_CHARS>(number);
            text_generator.generate(x, y, number_string, score_sprites);
        }

        /**
         * Sets score back to 0. Does NOT reset high score.
         */
        void resetScore() {
            score = 0;
        }

        int score; // current score
        int high_score; // best core
        bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> score_sprites; // Sprites to display scores
        bn::sprite_text_generator text_generator; // Text generator for scores
};

// Powerup types
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
    Powerup(int x, int y, PowerupType type, bn::size size) :
        sprite(bn::sprite_items::square.create_sprite(x, y)),
        active(true),
        powerup_type(type),
        size(size),
        bounding_box(create_bounding_box(sprite, size))
    {}

    void deactivate() {
        active = false;
    }

    bool active; // Whether this powerup is still collectible
    PowerupType powerup_type; // Which effect this powerup grants
    bn::sprite_ptr sprite;
    bn::size size; // The width and height of the sprite
    bn::rect bounding_box; // The rectangle around the sprite for checking collision
};

class Player {
    public:
        Player(int starting_x, int starting_y, bn::fixed player_speed, bn::size player_size) :
            sprite(bn::sprite_items::rat.create_sprite(starting_x, starting_y)),
            base_speed(player_speed),
            speed(player_speed), 
            size(player_size),
            bounding_box(create_bounding_box(sprite, size)),
            powerup_timer(0),
            has_powerup(false),
            is_invincible(false)
        {}
        /**
         * Update the position and bounding box of the player based on d-pad movement.
         * 
         * Also prevents the player from moving off the screen.
         */
        void update() {
            // Handle active powerup countdown
            if(has_powerup) {
                powerup_timer--;
                if(powerup_timer <= 0) {
                    has_powerup = false;
                    is_invincible = false;
                    speed = base_speed; // restore normal speed when powerup expires
                }
            }

            if(bn::keypad::right_held()) {
                sprite.set_x(sprite.x() + speed);
            }
            if(bn::keypad::left_held()) {
                sprite.set_x(sprite.x() - speed);
            }
            // TODO: Add logic for up and down (done)
            if(bn::keypad::down_held()) {
                sprite.set_y(sprite.y() + speed);
            }
            if(bn::keypad::up_held()) {
                sprite.set_y(sprite.y() - speed);
            }
            // prevents player from moving off the screen
            if(sprite.x() < MIN_X + size.width() / 2) {
                sprite.set_x(MIN_X + size.width() / 2);
            }
            if(sprite.x() > MAX_X - size.width() / 2) {
                sprite.set_x(MAX_X - size.width() / 2);
            }

            // prevents player from moving off the screen
            if(sprite.y() < MIN_Y + size.height() / 2) {
                sprite.set_y(MIN_Y + size.height() / 2);
            }
            if(sprite.y() > MAX_Y - size.height() / 2) {
                sprite.set_y(MAX_Y - size.height() / 2);
            }

            bounding_box = create_bounding_box(sprite, size);

        
        }

        /**
         * Apply a powerup effect to the player.
         */
        void applyPowerup(PowerupType type) {
            has_powerup = true;
            powerup_timer = POWERUP_DURATION;
            if(type == PowerupType::SPEED_BOOST) {
                speed = base_speed * 2; // double movement speed
                is_invincible = false;
            } else if(type == PowerupType::INVINCIBILITY) {
                is_invincible = true; // immune to enemy collision
                speed = base_speed;
            }
        }

                // Create the sprite. This will be moved to a constructor
        bn::sprite_ptr sprite;
        bn::fixed base_speed; // The normal speed of the player (before any powerup)
        bn::fixed speed; // The current speed of the player
        bn::size size; // The width and height of the sprite
        bn::rect bounding_box; // The rectangle around the sprite for checking collision
        int powerup_timer; // Frames remaining on the active powerup
        bool has_powerup; // Whether a powerup is currently active
        bool is_invincible; // Whether the player is currently invincible
    };

        class Enemy {
    public:
        Enemy(int starting_x, int starting_y, bn::fixed enemy_speed, bn::size enemy_size) :
            enemy_sprite(bn::sprite_items::cat.create_sprite(starting_x, starting_y)),
            speed(enemy_speed),
            size(enemy_size),
            bounding_box(create_bounding_box(enemy_sprite, size))
         {}

        /** moves enemy towards the player. if enemy catches player, enemy jumps to random spot */
        void update(Player& player) {
            bn::fixed dx = player.sprite.x() - enemy_sprite.x();
            bn::fixed dy = player.sprite.y() - enemy_sprite.y();

            // allows enemy to follow player
            if(dx > 0) {
                enemy_sprite.set_x(enemy_sprite.x() + speed);
            } else if(dx < 0) {
                enemy_sprite.set_x(enemy_sprite.x() - speed);
            }

            if(dy > 0) {
                enemy_sprite.set_y(enemy_sprite.y() + speed);
            } else if(dy < 0) {
                enemy_sprite.set_y(enemy_sprite.y() - speed);
            }

            // updates the bounding box to match the new enemy position
            bounding_box = create_bounding_box(enemy_sprite, size);

            // when the enemy catches the player, jump to a random spot on screen
            if(bounding_box.intersects(player.bounding_box)) {

             // Pick a random position within the screen bounds (with some padding for sprite size)
                int rand_x = rng.get_int(MIN_X + size.width(), MAX_X - size.width());
                int rand_y = rng.get_int(MIN_Y + size.height(), MAX_Y - size.height());
                enemy_sprite.set_x(rand_x);
                enemy_sprite.set_y(rand_y);
                bounding_box = create_bounding_box(enemy_sprite, size);
            }
        }

        /**
         * Push this enemy away from another enemy if they are overlapping.
         * Computes a repulsion vector based on the distance between the two enemies
         * and moves both apart by half the overlap, keeping enemies spread out.
         */
        void separateFrom(Enemy& other) {
            bn::fixed dx = enemy_sprite.x() - other.enemy_sprite.x();
            bn::fixed dy = enemy_sprite.y() - other.enemy_sprite.y();

            // Compute distance squared to check if enemies are too close
            bn::fixed dist_sq = dx * dx + dy * dy;
            bn::fixed sep = bn::fixed(ENEMY_SEPARATION);

            if(dist_sq < sep * sep && dist_sq > 0) {
                // Normalize the direction vector and push both enemies apart equally
                bn::fixed dist = bn::sqrt(dist_sq);
                bn::fixed push = (sep - dist) / 2;
                bn::fixed nx = dx / dist;
                bn::fixed ny = dy / dist;

                enemy_sprite.set_x(enemy_sprite.x() + nx * push);
                enemy_sprite.set_y(enemy_sprite.y() + ny * push);
                other.enemy_sprite.set_x(other.enemy_sprite.x() - nx * push);
                other.enemy_sprite.set_y(other.enemy_sprite.y() - ny * push);

                // updates the bounding boxes to match the new positions
                bounding_box = create_bounding_box(enemy_sprite, size);
                other.bounding_box = create_bounding_box(other.enemy_sprite, other.size);
            }
        }

         // create the sprite. This will be moved to a constructor
        bn::sprite_ptr enemy_sprite;
        bn::fixed speed; // The speed of the enemy
        bn::size size; // The width and height of the enemy sprite
        bn::rect bounding_box; // The rectangle around the enemy sprite for checking collision

        // random number generator for changing the spot  when the enemy catches the player
        bn::random rng; 
    };
        


        int main() {
            bn::core::init();

            // Create a new score display
            ScoreDisplay scoreDisplay = ScoreDisplay();

            // Create a player and initialize it
            // TODO: we will move the initialization logic to a constructor.
            Player player = Player(31, 19, 3.5, PLAYER_SIZE);
            // player.sprite.set_x(44);
            // player.sprite.set_y(22);
            // player.speed = 1.5;
            // player.size = PLAYER_SIZE;

            // bn::sprite_ptr enemy_sprite = bn::sprite_items::square.create_sprite(-30, 22);
            // bn::rect enemy_bounding_box = create_bounding_box(enemy_sprite, ENEMY_SIZE);

            // Create a vector of enemies with different starting positions and speeds.
            // Later enemies are faster than earlier ones.
            bn::vector<Enemy, 4> enemies; //4 enemies 
            enemies.push_back(Enemy( 25,  21, bn::fixed(1.0),  ENEMY_SIZE));
            enemies.push_back(Enemy(-25, -30, bn::fixed(1.5),  ENEMY_SIZE));
            enemies.push_back(Enemy( 60, -40, bn::fixed(2.0),  ENEMY_SIZE));
            enemies.push_back(Enemy(-20,  50, bn::fixed(2.75), ENEMY_SIZE));

            // Powerup management
            bn::vector<Powerup, 2> powerups; // at most 2 active powerups at once
            int powerup_spawn_timer = POWERUP_SPAWN_INTERVAL;
            bn::random rng; // random number generator for powerup spawn positions and types

            while(true) {
                player.update();
                for(Enemy& enemy : enemies) {
                    enemy.update(player);

                    // Reset the current score and player position if this enemy collides with the player
                    if(enemy.bounding_box.intersects(player.bounding_box) && !player.is_invincible) {
                        scoreDisplay.resetScore();
                        player.sprite.set_x(44);
                        player.sprite.set_y(22);
                        // bn::sound_items::hit.play(); 
                    }
                }

                // Separate enemies from each other (no overlapping)
                for(int i = 0; i < (int)enemies.size(); i++) {
                    for(int j = i + 1; j < (int)enemies.size(); j++) {
                        enemies[i].separateFrom(enemies[j]);
                    }
                }

                // Update the scores and disaply them
                scoreDisplay.update(); 
                

                bn::core::update();
            }
        }