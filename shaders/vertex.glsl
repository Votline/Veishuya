#version 330 core

layout (location = 0) in vec3 aPos;
uniform vec4 uTransform;
uniform vec3 uCamPos;

void main() {
	float posX = uTransform.x;
	float posY = uTransform.y;
	float scaleW = uTransform.z;
	float scaleH = uTransform.w;

  vec3 pos = vec3(aPos.x * scaleW + posX,aPos.y * scaleH + posY,aPos.z) - uCamPos;

  float perspective = 1.0 / (pos.z + 2.0);
  gl_Position = vec4(pos.x * perspective, pos.y * perspective, pos.z*0.1, 1.0);

}
