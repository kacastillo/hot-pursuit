#include "enemy.h"
#include "player.h"
#include "collision.h"

#include <bn_sprite_items_cat.h>
#include <bn_math.h>


// Enemy separation distance — if closer than this, push apart
static constexpr int ENEMY_SEPARATION = 14;

Enemy::Enemy(int starting_x, int starting_y,
             bn::fixed enemy_speed, bn::size enemy_size) :
    enemy_sprite(bn::sprite_items::cat.create_sprite(starting_x, starting_y)),
    bounding_box(create_bounding_box(enemy_sprite, enemy_size)),
    speed(enemy_speed),
    size(enemy_size)
{}

/** moves enemy towards the player. if enemy catches player, enemy jumps to random spot */
void Enemy::update(Player& player)
{
    bn::fixed dx = player.sprite.x() - enemy_sprite.x();
    bn::fixed dy = player.sprite.y() - enemy_sprite.y();

    if(dx > 0)
        enemy_sprite.set_x(enemy_sprite.x() + speed);
    else if(dx < 0)
        enemy_sprite.set_x(enemy_sprite.x() - speed);

    if(dy > 0)
        enemy_sprite.set_y(enemy_sprite.y() + speed);
    else if(dy < 0)
        enemy_sprite.set_y(enemy_sprite.y() - speed);

    // Update bounding box AFTER movement
    bounding_box = create_bounding_box(enemy_sprite, size);
}

/**
 * Push this enemy away from another enemy if they are overlapping.
 * Computes a repulsion vector based on the distance between the two enemies
 * and moves both apart by half the overlap, keeping enemies spread out.
 */
void Enemy::separateFrom(Enemy& other)
{
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
        bounding_box = create_bounding_box(enemy_sprite, size);
        other.bounding_box = create_bounding_box(other.enemy_sprite, other.size);
    }
}