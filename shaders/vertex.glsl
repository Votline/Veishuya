#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in uvec4 aBoneIndices;
layout (location = 2) in vec4 aBoneWeights;

uniform mat4 uMVP;
uniform mat4 uBones[64];
uniform bool uHasBones;

void main() {
  if (uHasBones) {
    mat4 boneTransform = uBones[aBoneIndices.x] * aBoneWeights.x;
    boneTransform += uBones[aBoneIndices.y] * aBoneWeights.y;
    boneTransform += uBones[aBoneIndices.z] * aBoneWeights.z;
    boneTransform += uBones[aBoneIndices.w] * aBoneWeights.w;
    vec4 skinnedPos = boneTransform * vec4(aPos, 1.0);
    gl_Position = uMVP * skinnedPos;
  } else {
    gl_Position = uMVP * vec4(aPos, 1.0);
  }
}
