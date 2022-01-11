#version 330

// From Vertex Shader
in vec3 vcolor;
in vec2 vpos; // Distance from local origin
in vec3 vpos3D;
in vec3 vnormal;
in mat3 vtransform;
in mat3 vtransform3D;

// Application data
uniform sampler2D sampler0;
uniform float time;
uniform vec3 fcolor;

// Output color
layout(location = 0) out vec4 color;

void main()
{
	// vec3 ambient = vec3(1.0, 0.5, 0.31);
    // vec3 diffuse = vec3(1.0, 0.5, 0.31);
    // vec3 specular = vec3(0.5, 0.5, 0.5);
    // float shininess = 32.0;
	// vec3 norm = normalize(vnormal);
    // vec3 lightDir = normalize(vec3(0.0, 0.0, 10.0) - vec3(vpos, 0.0));
    // float diff = dot(norm, lightDir);
	// if (diff < 0.0) {
	// 	diff = 0.0;
	// }
    // vec3 diffuse_2 = vec3(1.0, 1.0, 0.2) * (diff * diffuse);
    
    // // specular
    // vec3 viewDir = vec3(0.0, 0.0, -1.0); //normalize(viewPos - FragPos);
    // vec3 reflectDir = reflect(-lightDir, norm);  
	// float view_reflection = dot(viewDir, reflectDir);
	// if (view_reflection < 0.0) {
	// 	view_reflection = 0.0;
	// }
    // float spec = pow(view_reflection, shininess);
    // vec3 specular_2 = vec3(1.0, 1.0, 0.2) * (spec * specular);  
        
    // vec3 result = ambient + diffuse_2 + specular_2;
	// vec3 final_result = fcolor * vcolor * result * (3 - (0.6 * sin(time / 10)));
    vec3 lAmbient = vec3(1.0, 1.0, 1.0);
    vec3 lDiffuse = vec3(0.5, 0.5, 1.0);
    vec3 lSpecular= vec3(1.0, 1.0, 1.0);

    vec3 plPos = vec3(0.0, 3.0, 5.0);
    vec3 plDir = normalize(plPos - vec3(vpos.xy, vpos3D.z));

    vec3 n = normalize(vtransform3D * vnormal);
    vec3 l = normalize(vec3(vtransform * plDir));
    vec3 v = normalize(-vec3(vtransform * vpos3D));
    vec3 r = reflect(l, n);

    float lambert = dot(l, n);
    float ambientInt = 0.2;
    float specularInt = 0.5;
    float diffuseInt = 1.0;
    float shininess = 128.0;
    float thing = dot(r,v);

    if (thing < 0.0) {
        thing = 0.0;
    }
    float specular = pow(thing, shininess);
    vec3 final_result = vec3(
        lAmbient * ambientInt +
        lDiffuse * diffuseInt * lambert +
        lSpecular * specularInt * specular +
        vcolor * 0.001 + fcolor * 0.7);
	color = vec4(final_result, 1.0);
}