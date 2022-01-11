#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float darken_screen_factor;
uniform float is_light;

in vec2 texcoord;
in vec3 v_upper_left;
in vec3 v_upper_right;
in vec3 v_lower_left;
in vec3 v_lower_right;
in vec3 v_light_pos;
// in mat3 v_transform;

layout(location = 0) out vec4 color;

vec2 distort(vec2 uv) 
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE THE WATER WAVE DISTORTION HERE (you may want to try sin/cos)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	return uv;
}

vec4 color_shift(vec4 in_color) 
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE THE COLOR SHIFTING HERE (you may want to make it blue-ish)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	return in_color;
}

vec4 fade_color(vec4 in_color) 
{
	return in_color;
}

int ccw(vec2 A, vec2 B, vec2 C)
{
	if ((C.y-A.y) * (B.x-A.x) > (B.y-A.y) * (C.x-A.x)) {
		return 0;
	}
	return 1;
}

int doIntersect(vec2 A, vec2 B, vec2 C, vec2 D)
{
	if (ccw(A,C,D) != ccw(B,C,D)) {
		if (ccw(A,B,C) != ccw(A,B,D)) {
			return 0;
		}
	}
	return 1;
}

void main()
{
	vec2 coord = distort(texcoord);

    vec4 in_color = texture(screen_texture, coord);
    color = color_shift(in_color);
	if (is_light == 1) {
		// vec3 temp_frag = gl_FragCoord.xyz;
		// vec3 current_pos3 = v_transform * temp_frag;
		// vec2 current_pos = current_pos3.xy;
		vec2 current_pos = gl_FragCoord.xy;
		color = fade_color(color);
		vec3 light_pos = v_light_pos;//vec2((v_lower_right.xy + v_lower_left.xy) / 2.0);
		if (doIntersect(v_upper_left.xy, v_upper_right.xy, light_pos.xy, current_pos) == 0) {
    		color = color * vec4(0.2, 0.2, 0.2, 0.1);
		}
		if (doIntersect(v_upper_left.xy, v_lower_left.xy, light_pos.xy, current_pos) == 0) {
    		color = color * vec4(0.2, 0.2, 0.2, 0.1);
		}
		if (doIntersect(v_lower_left.xy, v_lower_right.xy, light_pos.xy, current_pos) == 0) {
    		color = color * vec4(0.2, 0.2, 0.2, 0.1);
		}
		if (doIntersect(v_upper_right.xy, v_lower_right.xy, light_pos.xy, current_pos) == 0) {
    		color = color * vec4(0.2, 0.2, 0.2, 0.1);
		}
	} else {
		color = fade_color(color);
	}

	// color = fade_color(color);
	// color.w = 0.5;
}
