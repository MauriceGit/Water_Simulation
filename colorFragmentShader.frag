#version 330 

in vec3 color; 
out vec4 Color; 
uniform vec3 colorIn; 

void main(){ 
	Color = vec4(colorIn, 1.0); 
}
