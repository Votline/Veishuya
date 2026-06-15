#ifndef RENDER_H
#define RENDER_H

// render_init initialize shaders
void render_init();

// render_clear changes color to 'rgba'
void render_clear(float r, float g, float b, float a);

// render_draw renders quad in (x;y) with (w;h) color (r;g;b;a)
void render_draw(int x, int y, int w, int h, float r, float g, float b, float a);

#endif
