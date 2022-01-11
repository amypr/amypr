#version 330

// Input attributes
in vec3 in_color;
in vec3 in_position;

out vData
{
	vec3 vcolor;
	// TODO add normal
	vec3 in_pos;
	int id;
}vertex;


// Application data
uniform mat3 transform;
uniform mat3 projection;

void main()
{
	vertex.in_pos = projection * vec3(in_position.xy,1.0);
	vertex.vcolor = in_color;
	vertex.id = gl_VertexID;
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}