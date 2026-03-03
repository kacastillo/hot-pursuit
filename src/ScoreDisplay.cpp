#include "ScoreDisplay.h"
#include "bn_sprite_items_common_fixed_8x16_font.h"
#include "bn_string.h"

ScoreDisplay::ScoreDisplay()
    : text_generator(bn::sprite_items::common_fixed_8x16_font)
{
    text_generator.set_center_alignment();
}

void ScoreDisplay::update(int score) {
    text_sprites.clear();
    text_generator.generate(
        0,
        -70,
        "Score: " + bn::to_string<16>(score),
        text_sprites
    );
}