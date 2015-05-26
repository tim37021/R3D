// by Nikos Papadopoulos, 4rknova / 2015
// Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

// Gaussian kernel
//  3  0  0
//  0 -1  0
//  0  0 -1
#version 330

uniform vec2 viewport;
uniform sampler2D text;
out vec4 color;

vec3 sample(const int x, const int y, in vec2 fragCoord)
{
    vec2 uv = fragCoord.xy / viewport * viewport;
	uv = (uv + vec2(x, y)) / viewport;
	return texture2D(text, uv).xyz;
}

vec3 filter(in vec2 fragCoord)
{
	vec3 sum = sample(-1, -1, fragCoord) * 3.
	+ sample( 0,  0, fragCoord) * -1.
	+ sample( 1,  1, fragCoord) * -1.;

	return sum;
}

void main()
{

	vec3 cf = filter(gl_FragCoord.xy);

	color = vec4(cf, 1);
}