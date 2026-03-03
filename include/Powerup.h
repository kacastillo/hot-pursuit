#ifndef POWERUP_H
#define POWERUP_H

#include "bn_sprite_ptr.h"
#include "bn_size.h"
#include "bn_rect.h"
#include "PowerupType.h"

class Powerup {
public:
    Powerup(int x, int y, PowerupType type, bn::size size);

    void update();
    bn::rect getBoundingBox() const;
    PowerupType getType() const;
    bn::sprite_ptr& getSprite();

private:
    bn::sprite_ptr sprite;
    bn::size size;
    bn::rect bounding_box;
    PowerupType type;
};

#endif