#version 430
layout (local_size_x = 1, local_size_y = 1) in;

writeonly uniform image2D destTex;

struct Ray
{
	vec3 pos;
	vec3 dir;
};

struct Sphere
{
	float radius;
	vec3 center;
	vec3 emission;
	vec3 color;
	float reflectance;
};

Sphere spheres[2] = Sphere [](
	Sphere(1.0, vec3(0.0), vec3(0.0), vec3(1.0), 0.05),
	Sphere(1, vec3(0, 0, -2), vec3(0.0), vec3(0.6, 0.4, 0.6), 0.05)
);

uniform vec2 viewport = vec2(800, 800);
uniform vec3 eyePos = vec3(0, 0, 5);
uniform vec3 eyeDir = vec3(0, 0, -1);
uniform vec3 eyeUp = vec3(0, 1, 0);
uniform float near = 0.5;
uniform float fov=45.0*3.1415926/180.0;
uniform sampler2D uTexRandom;
uniform vec2 randFactor;

Ray calcRayDir(vec2 coords)
{
	vec3 e_d = normalize(eyeDir);

	vec3 u = normalize(cross(e_d, eyeUp));
	vec3 v = cross(u, e_d);

	Ray r;

	vec3 center = eyePos+near*e_d;
	vec2 coords_ = coords-(viewport/2+vec2(0.5));
	vec2 pixel_width = 2.0*near*tan(fov/2.0)/viewport;
	r.pos = center+u*coords_.x*pixel_width.x+v*coords_.y*pixel_width.y;
	r.dir = normalize(r.pos-eyePos);

	return r;
}

float rand(vec2 co)
{
	return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453);
}

float RaySphereIntersect(in Ray r, in Sphere sp)
{
	float t;
	vec3 l = sp.center - r.pos;
	float s = dot(l, r.dir);
	float l2 = dot(l, l);
	float r2 = sp.radius*sp.radius;

	if(s<0 && l2 > r2)
		return -1;
	float m2 = l2 - s*s;
	if(m2>r2)
		return -1;
	float q = sqrt(r2 - m2);
	if(l2>r2)
		t = s - q;
	else
		t = s + q;
	return t;
}

mat3 TBN(in vec3 normal)
{
	vec3 rvec = texture(uTexRandom, gl_GlobalInvocationID.xy/viewport).xyz * 2.0 - 1.0;
	vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
	vec3 bitangent = cross(normal, tangent);
	return mat3(tangent, bitangent, normal);
}

void findNearestObject(in Ray r, out float t, out Sphere s)
{
	float tMin = 9999999;
	uint obj = -1;
	for(uint i=0; i<2; i++)
	{
		float t = RaySphereIntersect(r, spheres[i]);
		if(t>=0 && t<tMin)
		{
			tMin=t;
			obj=i;
		}
	}
	s = spheres[obj];
	t = (obj==-1? -1: tMin);
}

vec3 traceRay(in Ray r, in uint depth)
{
	vec3 color=vec3(0.0);

	const uint bounce_count=2;
	float t;
	Sphere sphere;
	float transmit=1.0;

	for(uint i=0; i<bounce_count; i++)
	{

		findNearestObject(r, t, sphere);
		if(t>=0)
		{
			vec3 p= r.pos + r.dir*t;
			vec3 n = normalize(p - sphere.center);

			vec3 l = normalize(vec3(0, 5, 0)-p);
			float diffuse=max(dot(n, l), 0.0);
			color += transmit*(sphere.emission + diffuse*sphere.color*vec3(0.8) + sphere.color * vec3(0.1));
			transmit *= sphere.reflectance;

			r.dir = reflect(r.dir, n);
			r.pos = p+0.0001*r.dir;
		}else
			break;
	}
	
	return color;
}

void main() {
	ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
	vec3 color=traceRay(calcRayDir(gl_GlobalInvocationID.xy+vec2(0.5)), 1);
	imageStore(destTex, storePos, vec4(pow(color, vec3(1.0/2.2)), 1.0));
}