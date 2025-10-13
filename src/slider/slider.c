#include "slider.h"

Slider slider_create(uint8_t x_min, uint8_t x_max, uint8_t y_min, uint8_t y_max)
{
    Slider s;
    s.x_min = x_min;
    s.x_max = x_max;
    s.y_min = y_min;
    s.y_max = y_max;
    s.position.x = 0.0;
    s.position.y = 0.0;
    return s;
}

void slider_update(Slider *s, uint8_t adc_x, uint8_t adc_y)
{
    // Scale ADC values to [0.0, 1.0] range
    s->position.x = scale(adc_x, s->x_min, s->x_max, 0.0, 1.0);
    s->position.y = scale(adc_y, s->y_min, s->y_max, 0.0, 1.0);
}

Vec2 slider_get_position(Slider *s)
{
    return s->position;
}

Vec2 slider_get_position_scaled(Slider *s, float min_x, float max_x, float min_y, float max_y)
{
    Vec2 scaled_position;
    scaled_position.x = scale(s->position.x, 0.0, 1.0, min_x, max_x);
    scaled_position.y = scale(s->position.y, 0.0, 1.0, min_y, max_y);
    return scaled_position;
}
