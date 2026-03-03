#include "Powerup.h"
#include "utils.h"
#include "bn_sprite_items_cheese.h"
#include "bn_sprite_items_square.h"

Powerup::Powerup(int x, int y, PowerupType type, bn::size size)
    : sprite(
        type == PowerupType::CHEESE
            ? bn::sprite_items::cheese.create_sprite(x, y)
            : bn::sprite_items::square.create_sprite(x, y)
      ),
      size(size),
      bounding_box(create_bounding_box(sprite, size)),
      type(type)
{
}

void Powerup::update() {
    bounding_box = create_bounding_box(sprite, size);
}

bn::rect Powerup::getBoundingBox() const {
    return bounding_box;
}

PowerupType Powerup::getType() const {
    return type;
}

bn::sprite_ptr& Powerup::getSprite() {
    return sprite;
}