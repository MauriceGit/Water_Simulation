#version 330 

in vec2 texcoords; 
in vec3 normal;
in float height;
in vec3 causticNormal;
uniform sampler2D texSand; 
uniform sampler2D texGrass; 
uniform sampler2D texRocks; 
uniform sampler2D texSnow; 
uniform sampler2D texForest; 
uniform sampler2D texTundra; 
out vec4 Color; 

float getLightValue(vec3 normalAt, vec3 sun, vec3 sun2, float lightUp) {
	float light  = max(0,dot(normalAt, sun));
	float light1 = max(0,dot(normalAt, sun2));
	return mix(min(1,light + lightUp), min(1,light1 + lightUp), 0.5);
}

vec4 calcTexChange (float height, float min, float max, vec4 tex1, vec4 tex2) {
	float aspect = (height-min) / (max-min);
	return mix(tex1, tex2, aspect);
}

void main(){ 	
	float 	sandGrassFrom 	= -3.0, sandGrassTo 		= 3.0,
			grassForestFrom = 4.0,  grassForestTo 	= 7.0,
			forestTundraFrom = 16.0, forestTundraTo = 25.0,
			tundraRocksFrom = 25.0, tundraRocksTo	= 30.0,
			rocksSnowFrom 	= 40.0, rocksSnowTo		= 60.0;
	
	vec3 sun  = vec3(1,1,1);
	vec3 sun2 = vec3(-1,1,1);
	vec4 sand  = texture(texSand , texcoords); 
	vec4 grass = texture(texGrass, texcoords*3.0); 
	vec4 forest = texture(texForest, texcoords*3.0);
	vec4 rocks = texture(texRocks, texcoords);
	vec4 tundra = texture(texTundra, texcoords*5.0);
	vec4 snow = texture(texSnow, texcoords*5.0);
	vec4 color;
	
	if (height < sandGrassFrom) {
		color = sand;
	} else if (height < sandGrassTo) {
		color = calcTexChange(height, sandGrassFrom, sandGrassTo, sand, grass);
	} else if (height < grassForestFrom) {
		color = grass;
	} else if (height < grassForestTo) {
		color = calcTexChange(height, grassForestFrom, grassForestTo, grass, forest);
	} else if (height < forestTundraFrom) {
		color = forest;
	} else if (height < forestTundraTo) {
		color = calcTexChange(height, forestTundraFrom, forestTundraTo, forest, tundra);
	} else if (height < tundraRocksFrom) {
		color = tundra;
	} else if (height < tundraRocksTo) {
		color = calcTexChange(height, tundraRocksFrom, tundraRocksTo, tundra, rocks);
	} else if (height < rocksSnowFrom) {
		color = rocks;
	} else if (height < rocksSnowTo) {
		color = calcTexChange(height, rocksSnowFrom, rocksSnowTo, rocks, snow);
	} else {
		color = snow;
	}
		
	color = color * getLightValue(normal, sun, sun2, 0.3);
	
	// Caustics
	//color = color * getLightValue(causticNormal, sun, sun2, 0.5);
	//float amount = dot(causticNormal, sun);
	//vec4 white = vec4(1,1,1,1);
	//vec4 caustics = mix(color, white, amount);
	
	Color = color;
}
