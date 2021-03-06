#version 330
// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform vec2 frame;

void main()
{
	texcoord = in_texcoord;
	texcoord.x += 0.125f * frame.x;
	// dialogue box has no need for this
	//texcoord.y += 0.5f * frame.y;

	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}