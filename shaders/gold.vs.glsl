#version 330

// Input attributes
in vec3 in_position;
in vec3 in_color;
in vec3 in_normal;

out vec3 vcolor;
out vec2 vpos;
out vec3 vpos3D;
out vec3 vnormal;
out mat3 vtransform;
out mat3 vtransform3D;

// Application data
uniform mat3 transform;
uniform mat3 transform3D;
uniform mat3 projection;

void main()
{
	vpos = in_position.xy; // local coordinated before transform
	vpos3D = in_position; // local coordinated before transform
	vcolor = in_color;// + (in_normal * 0.001);
	vnormal = in_normal;
	vtransform = transform;
	vtransform3D = transform3D;
	vec3 rotation = transform3D * in_position;
	vec3 pos = projection * transform * vec3(rotation.xy, 1.0); // why not simply *in_position.xyz ?
	gl_Position = vec4(pos.xyz, 1.0);
}