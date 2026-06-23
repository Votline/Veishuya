#ifndef VEISHIK_H
#define VEISHIK_H

#include "internal/objparser/parser.h"
#include "internal/render/render.h"

typedef struct {
  RenderObject render_object;
  int state;
  float start_x;
  float start_y;
  float target_x;
  float target_y;
  float anim_time;
  float base_speed;
  float current_speed;
  float idle_timer;
  float idle_duration;
} Veishik;

// veishik_init fill struct fields by pointer
void veishik_init(Veishik* cube, const char* model_path);

// veishik_update update object in the scene (timers, movement)
void veishik_update(Veishik* vei, float delta_time);

#endif
