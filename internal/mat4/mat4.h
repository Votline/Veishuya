#ifndef MAT4_H
#define MAT4_H

// mat4_identity creates matrix with default state (1.0)
void mat4_identity(float* m);

// mat4_mul multiplies matrix A by B with rule 'row per column'
void mat4_mul(float* res, float* A, float* B);

// mat4_model build tranform matrix for object
// Applied scale (sx, sy, sz)
// Transform object in abosulte coords (tx, ty, tz)
void mat4_model(float* m, float tx, float ty, float tz, float sx, float sy,
                float sz);

// mat4_perspective creates matrix perspective
// Adjuct camera visibility cone:
//   - fov_deg: fov (bigger - wider view)
//   - aspect: screen aspect ratio (winW/winH), protects scretching
//   - near_plane/far_plane: range of visibility
void mat4_perspective(float* m, float fov_deg, float aspect, float near_plane,
                      float far_plane);

void mat4_translate(float* m, float x, float y, float z);

void mat4_from_quat(float* m, const float* q);

#endif
