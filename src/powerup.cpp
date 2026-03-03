#include "powerup.h"
#include "bn_sprite_items_cheese.h"

/**
 * Creates a rectangle centered at a sprite's location with a given size.
 */
static bn::rect create_bounding_box(bn::sprite_ptr sprite, bn::size box_size) {
    return bn::rect(sprite.x().round_integer(), sprite.y().round_integer(),
                    box_size.width(), box_size.height());
}

Powerup::Powerup(int x, int y, PowerupType type, bn::size size)
    : sprite(bn::sprite_items::cheese.create_sprite(x, y)),
      active(true),
      powerup_type(type),
      powerup_size(size),
      bounding_box(create_bounding_box(sprite, size))
{}

void Powerup::deactivate() {
    active = false;
}
