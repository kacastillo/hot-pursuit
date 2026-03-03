#include "enemy.h"
#include <bn_math.h>
#include "bn_sprite_items_cat.h"

// Enemy separation distance — if closer than this, push apart
static constexpr int ENEMY_SEPARATION = 14;

/**
 * Creates a rectangle centered at a sprite's location with a given size.
 */
static bn::rect create_bounding_box(bn::sprite_ptr sprite, bn::size box_size) {
    return bn::rect(sprite.x().round_integer(), sprite.y().round_integer(),
                    box_size.width(), box_size.height());
}

Enemy::Enemy(int starting_x, int starting_y, bn::fixed enemy_speed, bn::size enemy_size)
    : enemy_sprite(bn::sprite_items::cat.create_sprite(starting_x, starting_y)),
      speed(enemy_speed),
      size(enemy_size),
      bounding_box(create_bounding_box(enemy_sprite, size))
{}

void Enemy::update(Player& player) {
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
}

void Enemy::separateFrom(Enemy& other) {
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