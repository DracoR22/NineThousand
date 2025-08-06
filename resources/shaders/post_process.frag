#version 330 core

out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;
uniform float gamma;
uniform float exposure;


// TONE MAPPING
vec3 uncharted2Tonemap(vec3 x) {
  float A = 0.15;
  float B = 0.50;
  float C = 0.10;
  float D = 0.20;
  float E = 0.02;
  float F = 0.30;
  float W = 11.2;
  return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

vec3 uncharted2(vec3 color) {
  const float W = 11.2;
  float exposureBias = 2.0;
  vec3 curr = uncharted2Tonemap(exposureBias * color);
  vec3 whiteScale = 1.0 / uncharted2Tonemap(vec3(W));
  return curr * whiteScale;
}

vec3 filmic(vec3 x) {
  vec3 X = max(vec3(0.0), x - 0.004);
  vec3 result = (X * (6.2 * X + 0.5)) / (X * (6.2 * X + 1.7) + 0.06);
  return pow(result, vec3(2.2));
}

vec3 reinhard(vec3 x) {
  return x / (1.0 + x);
}

vec3 ACES(vec3 x)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

#define AGX_LOOK 2


vec3 AGX(vec3 col)
{	
	col = mat3(.842, .0423, .0424, .0784, .878, .0784, .0792, .0792, .879) * col;
	// Log2 space encoding
	col = clamp((log2(col) + 12.47393) / 16.5, vec3(0), vec3(1));
	// Apply sigmoid function approximation
	col = .5 + .5 * sin(((-3.11 * col + 6.42) * col - .378) * col - 1.44);
	// AgX look (optional)
	#if AGX_LOOK == 1
	// Golden
	col = mix(vec3(dot(col, vec3(.216,.7152,.0722))), col * vec3(1.,.9,.5), .8);
	#elif AGX_LOOK == 2
	// Punchy
	col = mix(vec3(dot(col, vec3(.216,.7152,.0722))), pow(col,vec3(1.35)), 1.4);
	#endif
	
	return col;
}

void main()
{
    vec3 hdrColor = texture(screenTexture, TexCoords).rgb;
    vec3 bloom = texture(bloomTexture, TexCoords).rgb;

    vec3 colorWithBloom = hdrColor + bloom;

    vec3 toneMapped = uncharted2(colorWithBloom * exposure);
    toneMapped = pow(toneMapped, vec3(1.0 / gamma));

     FragColor = vec4(toneMapped, 1.0);
     //FragColor = texture(bloomTexture, TexCoords); 
}
