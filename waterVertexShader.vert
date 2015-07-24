#version 330 
		
layout (location = 0) in vec3 vertPos; 
layout (location = 1) in vec3 normal_in; 
layout (location = 2) in vec3 terrain;
uniform mat4 viewMatrix, projMatrix; 
uniform vec3 cameraPos; 
out vec3 pos_vec; 
out vec3 normal; 
out vec3 pos_cam; 
out float height;

void main(){ 
	vec3 newPos = vec3(vertPos - pos_cam); 
	gl_Position =  projMatrix * viewMatrix * vec4(vertPos, 1.0); 
	pos_vec = vertPos; 
	normal   = normalize(normal_in); 
	pos_cam = cameraPos; 
	height = terrain.y;
}
