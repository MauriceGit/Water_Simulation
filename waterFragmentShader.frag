#version 330 
		

in vec3 pos_vec; 
in vec3 normal; 
in vec3 pos_cam;
in vec3 backgroundColor;
in float height;
uniform sampler2D texsampler; 
uniform sampler2D texsamplerDepth; 
uniform samplerCube texsamplerCube; 
uniform mat4 viewMatrix, projMatrix; 
out vec4 Color; 

float linearizeDepth2 (float depth) {
	float nearPlane = 0.1, farPlane = 1000.0;
	return (2*nearPlane) / (farPlane + nearPlane - depth * (farPlane - nearPlane));
}

float linearizeDepth (float depth) {
	float nearPlane = 0.1, farPlane = 1000.0;
	float res = (2*nearPlane) / (farPlane + nearPlane - depth * (farPlane - nearPlane));
	res = (res+0.3)*(res+0.3) - 0.5;
	return res;
}

// Umwandlung einer Weltkoordinate in Screenkoordinate!
vec3 getScreenPos (vec3 worldPos) {
	vec4 screenPos 	= projMatrix * viewMatrix * vec4(worldPos, 1.0);
	vec3 screenPos3	= screenPos.xyz / screenPos.w;
	return (screenPos3 + vec3(1.0)) / 2.0;
}

vec3 cubeMapRefl (vec3 reflectionWorld) {
	return texture(texsamplerCube, reflectionWorld).rgb;
}

vec3 cubeMapRefr (vec3 refractionWorld) {
	return texture(texsamplerCube, refractionWorld).rgb;
}

// Tiefe des Wassers an dem Fragment:
vec3 calcWaterDependentDarkness(vec3 color, float maxDepth) {
	// zwischen 0 und 1.
	vec3 screenPos		= getScreenPos(pos_vec);
	vec2 screenTexPos 	= screenPos.xy;
	
	float texDepth = texture(texsamplerDepth, screenTexPos).x;
	float worldDepth = screenPos.z;
	
	float depth = worldDepth - texDepth;
	vec3 black = vec3(0.0);
	
	/*if (depth <= 1.0 && depth >= 0.0) {
		return vec3(1,0,0);
	} else
		return vec3(1.0);*/
	return vec3(texDepth);
	
	return mix(black, color, 1.0-depth);
}

vec3 raytrace(in vec3 reflectionWorld, in int maxCount, in float stepSize) {
	vec3 color = vec3(1.0); 	
	vec3 testVec = pos_vec;
	
	vec3 reflectionVector = reflectionWorld * stepSize;
	
	vec3 screenPos		= getScreenPos(testVec);
	vec2 screenTexPos 	= screenPos.xy;
	
	
	float texDepth = texture(texsamplerDepth, screenTexPos).x;
	float worldDepth = screenPos.z;
	
	bool run = true;
	int count = 0;

	while (run) {
		
		texDepth = texture(texsamplerDepth, screenTexPos).x;
		worldDepth = screenPos.z;
		
		if (texDepth <= worldDepth) {
			color = texture(texsampler, screenTexPos).rgb;
			break;
		}
		
		testVec 		= testVec + reflectionVector;
		screenPos		= getScreenPos(testVec);
		screenTexPos 	= screenPos.xy;
		
		count = count+1;
		run = 	screenTexPos.x < 1.0 && screenTexPos.x >= 0.0 &&
				screenTexPos.y < 1.0 && screenTexPos.y >= 0.0 && count < maxCount;
	}
	
	if (!run) {
		color = cubeMapRefl(testVec);
		// Cheating
		//color = vec3(0,1,1);
	}
	
	return color;
} 

float getFresnelFactor (void) {
	float fresnelPower = 3; 
	float fresnelScale = 5.0; 
	float fresnelBias  = 0.05373; 
	return fresnelBias + fresnelScale * pow(1 + dot(normalize(pos_vec - pos_cam), normal), fresnelPower);
}

float calcStepSize(float stepSize, float max, vec3 n, vec3 cam) {
	float scalarProduct = dot(n, cam);
	float res = 0.0;
	
	res = (1.0 - scalarProduct) * (max-stepSize);
	
	return res + stepSize;
}

void main(){ 
	const int maxCount = 250;
	const float ratio = 1.0 / 1.3333; 
	float stepSize = 20.0;
	float maxStepSize = 400.0;
	float maxDepth = 0.9;
	vec4 black = vec4(0,0,0,0);
	
	vec3 eyePosition = normalize(pos_vec - pos_cam);
	
	vec3 reflectionWorld = normalize (reflect(eyePosition, normal));
	vec3 refractionWorld = normalize (refract(eyePosition, normal, ratio));
	
	// Nicht physikalisch korrekte Anpassung, dass mehr Geometrie gespiegelt wird!
	reflectionWorld = reflectionWorld + vec3(0, -0.15, 0);
	
	// Stepsize anpassen an das Verhältnis der Normalen zum Blickvektor
	stepSize = calcStepSize(stepSize, maxStepSize, -normal, eyePosition);
	
	vec3 reflectedColor = raytrace(reflectionWorld, maxCount, stepSize); 
	vec3 refractedColor = raytrace(refractionWorld, maxCount, stepSize);
	
	//refractedColor = calcWaterDependentDarkness(refractedColor, maxDepth);
	
	vec4 screenSpaceReflection = vec4(reflectedColor, 1.0);
	vec4 screenSpaceRefraction = vec4(refractedColor, 1.0);
	
	screenSpaceRefraction = mix(black, screenSpaceRefraction, 0.5);
	
	vec4 tmpColor = mix (screenSpaceRefraction, screenSpaceReflection, getFresnelFactor());
	tmpColor = mix (black, tmpColor, 0.8);
	
	// Wenn das Wasser tief wird.
	if (height < 0.0) {
		float aspect = (height+8.0) / 8.0;
		tmpColor = mix(black, tmpColor, aspect);
		tmpColor = vec4(0.0);
	}
	
	Color = tmpColor;
	//Color = screenSpaceReflection;
	//Color = screenSpaceRefraction;
}
