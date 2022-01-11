#version 330

// Input attributes
layout (location = 6) in vec2 aOffset;
layout (location = 7) in vec2 lifespan;
layout (location = 8) in vec2 velocity;

in vec3 in_color;
in vec3 in_position;

out vec3 vcolor;
out vec2 life;
// Application data
uniform mat3 transform;
uniform mat3 projection;

void main()
{
	vcolor = in_color;
	vec2 displacement = (aOffset + velocity) * (lifespan[1] - lifespan[0])/lifespan[1];
	vec3 pos = projection * transform * vec3(in_position.xy + displacement, 1.0);
	life = lifespan;
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}