#ifndef SCOREDISPLAY_H
#define SCOREDISPLAY_H

#include "bn_sprite_text_generator.h"
#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_fixed_point.h"

class ScoreDisplay {
public:
    ScoreDisplay();

    void update(int score);

private:
    bn::sprite_text_generator text_generator;
    bn::vector<bn::sprite_ptr, 32> text_sprites;
};

#endif