#include "veishik.h"
#include<stdlib.h>
#include<math.h>

#define STATE_IDLE 0
#define STATE_MOVING 1

// get_random_float returns a random float between min and max
static float get_random_float(float min, float max) {
  return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

// lerp returns a linear interpolation between start and end
static float lerp(float start, float end, float t) {
  return start + (end - start) * t;
}

// veishik_init initializes veishik
void veishik_init(Veishik* veishik, Object* obj) {
  Veishik vei = *veishik;
  Object vei_data = *obj;
  vei.obj.bounds = vei_data.bounds;
  vei.obj.color = vei_data.color;
  
  vei.state = STATE_IDLE;
  vei.anim_time = 0.0f;
  vei.base_speed = 0.5f;
  vei.current_speed = vei.base_speed;

  vei.idle_timer = 0.0f;
  vei.idle_duration = 1.5f;

  *veishik = vei;
}

// veishik_update updates veishik position and state
void veishik_update(Veishik* veishik, float delta_time) {
  Veishik vei = *veishik;

  switch (vei.state) {
    case STATE_IDLE:
      vei.idle_timer+= delta_time;
      if (vei.idle_timer > vei.idle_duration) {
        if (rand() % 2 == 0) {
          vei.state = STATE_MOVING;
          vei.start_x = vei.obj.bounds.x;
          vei.start_y = vei.obj.bounds.y;
          vei.target_x = get_random_float(-0.8f, 0.8f);
          vei.target_y = get_random_float(-0.8f, 0.8f);
          vei.anim_time = 0.0f;

          float dx = vei.target_x - vei.start_x;
          float dy = vei.target_y - vei.start_y;
          float distance = hypotf(dx, dy);

          if (distance > 0.001f) {
            vei.current_speed = vei.base_speed / distance;
          } else {
            vei.current_speed = vei.base_speed;
          }
        } else {
          vei.idle_timer = 0.0f;
          vei.idle_duration = get_random_float(0.5f, 2.0f);
        }
      }
      break;

    case STATE_MOVING:
      vei.anim_time += delta_time * vei.current_speed;
      if (vei.anim_time > 1.0f) vei.anim_time = 1.0f;

      vei.obj.bounds.x = lerp(vei.start_x, vei.target_x, vei.anim_time);
      vei.obj.bounds.y = lerp(vei.start_y, vei.target_y, vei.anim_time);

      if (vei.anim_time >= 1.0f) {
        vei.state = STATE_IDLE;
        vei.idle_timer = 0.0f;
        vei.idle_duration = get_random_float(0.5f, 2.0f);
      }
      break;
  }
  *veishik = vei;
}
