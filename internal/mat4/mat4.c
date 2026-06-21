#include <math.h>

void mat4_identity(float* m) {
  for (int i = 0; i < 16; i++) m[i] = 0.0f;
  m[0] = 1.0f;
  m[5] = 1.0f;
  m[10] = 1.0f;
  m[15] = 1.0f;
}

void mat4_mul(float* res, float* A, float* B) {
  float tmp[16];
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      tmp[i + j * 4] =
          A[i + 0 * 4] * B[0 + j * 4] + A[i + 1 * 4] * B[1 + j * 4] +
          A[i + 2 * 4] * B[2 + j * 4] + A[i + 3 * 4] * B[3 + j * 4];
    }
  }
  for (int i = 0; i < 16; i++) res[i] = tmp[i];
}

void mat4_model(float* m, float tx, float ty, float tz, float sx, float sy,
                float sz) {
  mat4_identity(m);
  m[0] = sx;
  m[5] = sy;
  m[10] = sz;
  m[12] = tx;
  m[13] = ty;
  m[14] = tz;
}

void mat4_perspective(float* m, float fov_deg, float aspect, float near_plane,
                      float far_plane) {
  for (int i = 0; i < 16; i++) m[i] = 0.0f;
  float fov_rad = fov_deg * 3.14159265f / 180.0f;
  float tanHalfFov = tanf(fov_rad / 2.0f);

  m[0] = 1.0f / (aspect * tanHalfFov);
  m[5] = 1.0f / tanHalfFov;
  m[10] = -(far_plane + near_plane) / (far_plane - near_plane);
  m[11] = -1.0f;
  m[14] = -(2.0f * far_plane * near_plane) / (far_plane - near_plane);
}
