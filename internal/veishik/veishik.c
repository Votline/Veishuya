#include "veishik.h"

#include <math.h>
#include <stdlib.h>

#include "internal/render/render.h"

enum { STATE_IDLE = 0 };
enum { STATE_MOVING = 1 };
#define RANDOM_MAX 4294967295.0f

// get_random_float returns a random float between min and max
static float get_random_float(float min, float max) {
  float scale = (float)arc4random() / RANDOM_MAX;
  return min + scale * (max - min);
}

// lerp returns a linear interpolation between start and end
static float lerp(float start, float end, float t) {
  return start + (end - start) * t;
}

// veishik_init initializes veishik
void veishik_init(Veishik* veishik, const char* model_path) {
  veishik->state = STATE_IDLE;
  veishik->anim_time = 0.0f;
  veishik->base_speed = 0.5f;
  veishik->current_speed = veishik->base_speed;

  veishik->idle_timer = 0.0f;
  veishik->idle_duration = 1.5f;

  RenderObject render_obj = render_create_model(model_path);

  Bounds bounds = veishik->render_object.bounds;
  float scale = veishik->render_object.scale;

  veishik->render_object = render_obj;
  veishik->render_object.bounds = bounds;
  veishik->render_object.scale = scale;
}

// veishik_update updates veishik position and state
void veishik_update(Veishik* veishik, float delta_time) {
  switch (veishik->state) {
    case STATE_IDLE:
      veishik->idle_timer += delta_time;
      if (veishik->idle_timer > veishik->idle_duration) {
        if (rand() % 2 == 0) {
          veishik->state = STATE_MOVING;
          veishik->start_x = veishik->render_object.bounds.x;
          veishik->start_y = veishik->render_object.bounds.y;
          veishik->target_x = get_random_float(-0.8f, 0.8f);
          veishik->target_y = get_random_float(-0.8f, 0.8f);
          veishik->anim_time = 0.0f;

          float dx = veishik->target_x - veishik->start_x;
          float dy = veishik->target_y - veishik->start_y;
          float distance = hypotf(dx, dy);

          if (distance > 0.001f) {
            veishik->current_speed = veishik->base_speed / distance;
          } else {
            veishik->current_speed = veishik->base_speed;
          }
        } else {
          veishik->idle_timer = 0.0f;
          veishik->idle_duration = get_random_float(0.5f, 2.0f);
        }
      }
      break;

    case STATE_MOVING:
      veishik->anim_time += delta_time * veishik->current_speed;
      if (veishik->anim_time > 1.0f) veishik->anim_time = 1.0f;

      veishik->render_object.bounds.x =
          lerp(veishik->start_x, veishik->target_x, veishik->anim_time);
      veishik->render_object.bounds.y =
          lerp(veishik->start_y, veishik->target_y, veishik->anim_time);

      if (veishik->anim_time >= 1.0f) {
        veishik->state = STATE_IDLE;
        veishik->idle_timer = 0.0f;
        veishik->idle_duration = get_random_float(0.5f, 2.0f);
      }
      break;
  }
}
