<<<<<<< HEAD
#include "enemy.h"
=======
#include "utils.h"
#include "Player.h"
#include "Enemy.h"
#include "PowerupType.h"
#include "ScoreDisplay.h"
>>>>>>> 3fe3d9eb049acc0571fc28e33727dc5666bf3ca8

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
static constexpr int ENEMY_SPAWN_INTERVAL = 600; // 15 seconds

// Enemy separation distance — if closer than this, push apart
static constexpr int ENEMY_SEPARATION = 14;


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
// class ScoreDisplay {
//     public:
//         ScoreDisplay() :
//             score(0), // Start score at 0
//             high_score(0), // Start high score at 0
//             score_sprites(bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS>()), // Start with empty vector for score sprites
//             text_generator(bn::sprite_text_generator(common::fixed_8x16_sprite_font)) // Use a new text generator
//         {

//         }

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

/**
 * Types of powerups available in the game. SPEED_BOOST doubles player speed AND
 *  INVINCIBILITY makes player immune to enemies.
 */
// enum class PowerupType {
//     SPEED_BOOST,
//     INVINCIBILITY
// };

/**
 * A collectible powerup that temporarily grants the player either a speed boost or invincibility.
 * Spawns at a fixed position and is removed when collected by the player.
 */
// class Powerup {
// public:
//     Powerup(int x, int y, PowerupType type, bn::size powerup_size) :
//         sprite(bn::sprite_items::cheese.create_sprite(x, y)),
//         active(true),
//         powerup_type(type),
//         powerup_size(powerup_size),
//         bounding_box(create_bounding_box(sprite, powerup_size))
//     {}

    void deactivate() {
        active = false;
    }

    bn::sprite_ptr sprite;
    bool active; // Whether this powerup is still collectible
    PowerupType powerup_type; // Which effect this powerup grants
    bn::size powerup_size; // The width and height of the sprite
    bn::rect bounding_box; // The rectangle around the sprite for checking collision
};

/**
 * The player character, controlled by the user. Can move in 4 directions and collect powerups.
 * Has a bounding box for collision detection and state for active powerups.
 */
// class Player {
//     public:
//         Player(int starting_x, int starting_y, bn::fixed player_speed, bn::size player_size) :
//             sprite(bn::sprite_items::rat.create_sprite(starting_x, starting_y)),
//             base_speed(player_speed),
//             speed(player_speed), 
//             size(player_size),
//             bounding_box(create_bounding_box(sprite, size)),
//             powerup_timer(0),
//             has_powerup(false),
//             is_invincible(false)
//         {}
        /**
         * Update the position and bounding box of the player based on d-pad movement.
         * Also prevents the player from moving off the screen.
         */
        void update() {
            // Handle active powerup countdown
            if(has_powerup) {
                powerup_timer--;
                if(powerup_timer <= 0) {
                    has_powerup = false;
                    is_invincible = false;
                    // restore normal speed when powerup expires
                    speed = base_speed; 
                }
            }

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


            // prevents player from moving off the screen X
            if(sprite.x() < MIN_X + size.width() / 2) {
                sprite.set_x(MIN_X + size.width() / 2);
            }
            if(sprite.x() > MAX_X - size.width() / 2) {
                sprite.set_x(MAX_X - size.width() / 2);
            }

            // prevents player from moving off the screen Y
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
        
/**
 * Main game loop. Initializes player, score display, and enemy vector. Each frame, updates player movement, checks for collisions with enemies and powerups, updates enemy positions, and manages score display. 
 * 
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
scoreDisplay.text_generator.generate(0, 0, "PRESS START!!!",screen2_sprites);
while (bn::keypad::start_held()) {
    bn::core::update();
}
while(!bn::keypad::start_pressed()) {
    bn::core::update();
}
screen2_sprites.clear();

/**
     * Powerup management
     *  random number generator for powerup spawn positions and types
     *  at most 2 active powerups at once
     */
    
    bn::vector<Powerup, 2> powerups; 
    int powerup_spawn_timer = POWERUP_SPAWN_INTERVAL;
    bn::random rng; 
bn::sprite_ptr home_sprite = bn::sprite_items::home.create_sprite(80, 0); 

Player player = Player(80, 0, 3.5, PLAYER_SIZE);

    // Create a vector of enemies with different starting positions and speeds.
    // Later enemies are faster than earlier ones.
    bn::vector<Enemy, 4> enemies; //4 enemies 
    enemies.push_back(Enemy( 25,  21, bn::fixed(.75),  ENEMY_SIZE));

    int enemy_spawn_timer = ENEMY_SPAWN_INTERVAL; // Timer for spawning new enemies
    int enemy_jump_timer = 360;
    bn::fixed enemy_speed[4] = {bn::fixed(.75), bn::fixed(1.0), bn::fixed(1.5), bn::fixed(1.75)}; // Speeds for each enemy slot


    while(true) {
        player.update();


        bn::rect home_box = create_bounding_box(home_sprite, bn::size(16, 16));
        bool in_home = home_box.intersects(player.bounding_box);
        if (in_home) 
        {
            player.is_invincible = true; 

        } else if(!player.has_powerup) 
        {
            player.is_invincible = false; 
        }
    // Update enemies and check for collisions with player
        enemy_jump_timer--;
        if(enemy_jump_timer <= 0) {
            enemy_jump_timer = 360;
            int jx = rng.get_int(MIN_X + 16, MAX_X - 16);
            int jy = rng.get_int(MIN_Y + 16, MAX_Y - 16);
            enemies[0].enemy_sprite.set_x(jx);
            enemies[0].enemy_sprite.set_y(jy);
        }
        enemy_spawn_timer--;
        if(enemy_spawn_timer <= 0 && enemies.size() < enemies.max_size()) {
            enemy_spawn_timer = ENEMY_SPAWN_INTERVAL;
            // Spawn a new enemy in a random location with the next speed in the list
            int ex = rng.get_int(MIN_X + 16, MAX_X - 16);
            int ey = rng.get_int(MIN_Y + 16, MAX_Y - 16);
            enemies.push_back(Enemy(ex, ey, enemy_speed[enemies.size() - 1], ENEMY_SIZE));
        }

        for(Enemy& enemy : enemies) {
            if(!in_home) {
                enemy.update(player);
            }
            if(!player.is_invincible && enemy.bounding_box.intersects(player.bounding_box)) {
                scoreDisplay.resetScore();
                player.sprite.set_x(44);
                player.sprite.set_y(22);
                while(enemies.size() > 1) {
                    enemies.pop_back(); 
            }
            enemy_spawn_timer = ENEMY_SPAWN_INTERVAL; // Reset enemy spawn timer on player death
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


        
        


    
            // random choose between speed boost and invincibility
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

        // Update the scores and disaply them
        if(!in_home) {
        scoreDisplay.update(); 
        }

        bn::core::update();
    }
}