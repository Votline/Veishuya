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

void mat4_translate(float* mat, float x, float y, float z) {
  mat[0] = 1.0f;
  mat[1] = 0.0f;
  mat[2] = 0.0f;
  mat[3] = 0.0f;
  mat[4] = 0.0f;
  mat[5] = 1.0f;
  mat[6] = 0.0f;
  mat[7] = 0.0f;
  mat[8] = 0.0f;
  mat[9] = 0.0f;
  mat[10] = 1.0f;
  mat[11] = 0.0f;

  mat[12] = x;
  mat[13] = y;
  mat[14] = z;
  mat[15] = 1.0f;
}

void mat4_from_quat(float* mat, const float* q) {
  float x = q[0], y = q[1], z = q[2], w = q[3];

  float xx = x * x, xy = x * y, xz = x * z, xw = x * w;
  float yy = y * y, yz = y * z, yw = y * w;
  float zz = z * z, zw = z * w;

  mat[0] = 1.0f - 2.0f * (yy + zz);
  mat[1] = 2.0f * (xy + zw);
  mat[2] = 2.0f * (xz - yw);
  mat[3] = 0.0f;

  mat[4] = 2.0f * (xy - zw);
  mat[5] = 1.0f - 2.0f * (xx + zz);
  mat[6] = 2.0f * (yz + xw);
  mat[7] = 0.0f;

  mat[8] = 2.0f * (xz + yw);
  mat[9] = 2.0f * (yz - xw);
  mat[10] = 1.0f - 2.0f * (xx + yy);
  mat[11] = 0.0f;

  mat[12] = 0.0f;
  mat[13] = 0.0f;
  mat[14] = 0.0f;
  mat[15] = 1.0f;
}
