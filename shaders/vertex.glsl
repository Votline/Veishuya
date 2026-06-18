#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 transform;
uniform vec3 uCamPos;

void main() {
	float posX = transform.x;
	float posY = transform.y;
	float scaleW = transform.z;
	float scaleH = transform.w;

  vec3 pos = vec3(aPos.x * scaleW + posX,aPos.y * scaleH + posY,aPos.z) - uCamPos;

  float perspective = 1.0 / (pos.z + 2.0);
  gl_Position = vec4(pos.x * perspective, pos.y * perspective, pos.z*0.1, 1.0);

}
