#version 330
// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform int stun;
uniform int poisoned;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	if (stun == 1)
	{
		// 0.8 is just to make it not too strong
		color.xyz += (0.5) * 0.8 * vec3(1.0, 0.0, 0.0);
	}
	if (poisoned == 1)
	{
		// 0.8 is just to make it not too strong
		color.xyz += (0.5) * 0.8 * vec3(0.0, 1.0, 0.0);
	}
}
