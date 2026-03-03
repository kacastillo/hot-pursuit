// ScoreDisplay.h
#pragma once
#include "bn_sprite_text_generator.h"
#include "bn_sprite_items_common_fixed_8x16_font.h"
#include "bn_vector.h"

constexpr int MAX_SCORE_CHARS = 22;

class ScoreDisplay
{
public:
    ScoreDisplay();
    void update(int score);
    void resetScore();
private:
    void show_number(int x, int y, int number);

    int score;
    int high_score;
    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> score_sprites;
    bn::sprite_text_generator text_generator;
};