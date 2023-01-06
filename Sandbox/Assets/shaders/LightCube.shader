#type vertex

#version 450
layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}


#type fragment

#version 450
out vec4 FragColor;
out uint entityId; 

void main() {
	FragColor = vec4(1.0);
	entityId = -1;
}