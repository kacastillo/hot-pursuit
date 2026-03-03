#include "ScoreDisplay.h"
#include "bn_string.h"

ScoreDisplay::ScoreDisplay() :
    score(0),
    high_score(0),
    score_sprites(),
    text_generator(bn::sprite_items::common_fixed_8x16_font)
{
    text_generator.set_center_alignment();
}

void ScoreDisplay::update(int score)
{
    score_sprites.clear();
    text_generator.generate(0, -70, "Score: " + bn::to_string<16>(score), score_sprites);
}

void ScoreDisplay::resetScore()
{
    score = 0;
}