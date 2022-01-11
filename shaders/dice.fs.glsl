#version 330

// From Geo Shader
in fData
{
    vec3 gcolor;
}fragment;

// Application data
uniform vec3 fcolor;

// Output color
layout(location = 0) out vec4 color;

void main()
{
	color = vec4(fcolor * fragment.gcolor, 1.0);
}