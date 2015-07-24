#version 330 
		
layout (location = 0) in vec3 vertPos; 
layout (location = 1) in vec2 tex; 
layout (location = 2) in vec3 normalIn; 
layout (location = 3) in vec3 causticNormal;
uniform mat4 viewMatrix, projMatrix; 
out vec2 texcoords; 
out vec3 normal;
out float height;
out vec3 caustic;

void main(){ 
	gl_Position =  projMatrix * viewMatrix * vec4(vertPos, 1.0); 
	texcoords = tex; 
	normal = normalize(normalIn);
	height = vertPos.y;
	caustic = normalize(causticNormal);
}
