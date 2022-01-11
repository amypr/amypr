#version 330

layout (triangles) in;
layout (triangle_strip,max_vertices = 4) out;

in vData
{
	vec3 vcolor;
    vec3 in_pos;
    int id;
}verticies[];

uniform mat3 transform;
uniform mat3 projection;
uniform float time;
uniform float startTime;
uniform float endTime;
uniform int boom;

out fData
{
    vec3 gcolor;
}fragment;

// source: Chris Wellons 2018, triple32() function is a very low bias hash function, https://nullprogram.com/blog/2018/07/31/
uint triple32(uint x)
{
    x ^= x >> 17;
    x *= 0xed5ad4bbU;
    x ^= x >> 11;
    x *= 0xac4c1b51U;
    x ^= x >> 15;
    x *= 0x31848babU;
    x ^= x >> 14;
    return x;
}

vec4 explode(vec4 position, bool peak)
{
    // values
    float magnitude = 0.01;
    float approxPI =  3.14159265358979324;
    // time delta
    float time2 = time - startTime;

    // sort of random noise values for seemingly non uniform explosion paths of rectangles
    float noise1 = mod(int(triple32(uint(verticies[0].id))),int(3));
    float noise2 = mod(int(triple32(uint(verticies[2].id))),int(3));

    // These look better until the object moves and then its unstable
    // float noise1 = triple32(gl_in[0].gl_Position.x - gl_in[2].gl_Position.x);
    // float noise2 = triple32(gl_in[2].gl_Position.y - gl_in[1].gl_Position.y);
    // float noise3 = triple32(gl_in[1].gl_Position.y - gl_in[0].gl_Position.y);
    
    // choose direction
    vec3 direction_bias = vec3(1.0,1.0,0.0);

    // sine wave params
    float offset = 3.0;
    float amplitude = 3.0;
    float freq = 1.0/9000.0;
    float phase = 0;
    
    // time based explosion path
    vec3 time_wave = vec3((offset + amplitude*sin(2.0*approxPI*freq*time2 + phase)),(offset + amplitude*sin(2.0*approxPI*freq*time2 + phase)),1.0f);
    if(peak == true){
        time_wave = vec3((offset + amplitude*sin(2.0*approxPI*freq*(endTime-startTime) + phase)),(offset + amplitude*sin(2.0*approxPI*freq*(endTime-startTime) + phase)),1.0f);
    }
    // choose direction
    if(noise2 > 1.5){
        direction_bias.x = -1.0;
    }
    if(noise1 > 1.5){
        direction_bias.y = -1.0;
    }
    vec4 direction = position + vec4((vec3(noise1*0.1,noise2*0.1,1.0) * direction_bias * time_wave * magnitude).xy,0.0,0.0); 
    
    return direction;
}


void main() {  
    // set color
    fragment.gcolor = verticies[0].vcolor;  

    if(boom == 0){ // unexploded
        int i;
        for(i = 0;i < gl_in.length();i++)
        {
            fragment.gcolor = verticies[i].vcolor;  
            gl_Position = gl_in[i].gl_Position;
            EmitVertex();
        }
    }
    if(boom == 1){ // exploding
        gl_Position = explode(gl_in[0].gl_Position + vec4(-0.005,0.01,0.0,0.0),false);
        EmitVertex();
        gl_Position = explode(gl_in[0].gl_Position + vec4(-0.005,-0.01,0.0,0.0),false);
        EmitVertex();
        gl_Position = explode(gl_in[0].gl_Position + vec4(0.005,0.01,0.0,0.0),false);
        EmitVertex();
        gl_Position = explode(gl_in[0].gl_Position + vec4(0.005,-0.01,0.0,0.0),false);
        EmitVertex();
    }
    if(boom == 2){ // peek explosion position (finished effect)
        gl_Position = explode(gl_in[0].gl_Position + vec4(-0.005,0.01,0.0,0.0),true);
        EmitVertex();
        gl_Position = explode(gl_in[0].gl_Position + vec4(-0.005,-0.01,0.0,0.0),true);
        EmitVertex();
        gl_Position = explode(gl_in[0].gl_Position + vec4(0.005,0.01,0.0,0.0),true);
        EmitVertex();
        gl_Position = explode(gl_in[0].gl_Position + vec4(0.005,-0.01,0.0,0.0),true);
        EmitVertex();
    }

    EndPrimitive();
}  