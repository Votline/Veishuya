#ifndef RENDER_H
#define RENDER_H

// render_init initialize shaders
void render_init();

// render_clear changes color to 'rgba'
void render_clear(float r, float g, float b, float a);

// render_draw renders primitive shapes
void render_draw();

#endif
