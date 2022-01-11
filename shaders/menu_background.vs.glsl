#version 330


uniform float is_light;
uniform mat3 transform;
// uniform mat3 projection;

in vec3 in_position;
in vec2 upper_left;
in vec2 upper_right;
in vec2 lower_left;
in vec2 lower_right;
in vec2 light_pos;

out vec2 texcoord;
out vec3 v_upper_left;
out vec3 v_upper_right;
out vec3 v_lower_left;
out vec3 v_lower_right;
out vec3 v_light_pos;
// out mat3 v_transform;

void main()
{
    vec3 pos = in_position;
    if (is_light == 1) {
        // v_upper_left = projection * transform * vec3(upper_left.xy, 1.0);
        // v_upper_right = projection * transform * vec3(upper_right.xy, 1.0);
        // v_lower_left = projection * transform * vec3(lower_left.xy, 1.0);
        // v_lower_right = projection * transform * vec3(lower_right.xy, 1.0);
        // v_light_pos = projection * transform * vec3(light_pos.xy, 1.0);
        v_upper_left = transform * vec3(upper_left.xy, 1.0);
        v_upper_right = transform * vec3(upper_right.xy, 1.0);
        v_lower_left = transform * vec3(lower_left.xy, 1.0);
        v_lower_right = transform * vec3(lower_right.xy, 1.0);
        v_light_pos = transform * vec3(light_pos.xy, 1.0);

	    // pos = projection * transform * vec3(in_position.xy, 1.0); // why not simply *in_position.xyz ?
        // v_transform = transform;
    }
    gl_Position = vec4(pos.xy, in_position.z, 1.0);
	texcoord = (pos.xy + 1) / 2.f;
}
