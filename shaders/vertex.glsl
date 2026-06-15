#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 transform;

void main() {
	float posX = transform.x;
	float posY = transform.y;
	float scaleW = transform.z;
	float scaleH = transform.w;

	gl_Position = vec4(
		(aPos.x * scaleW) + posX,
		(aPos.y * scaleH) + posY,
		aPos.z, 1.0
	);
}
