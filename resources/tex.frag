#version 330 core
// mandatory: final output color
out vec4 FragColor; 

in vec3 ourColor;
in VS_OUT {
    vec2 texCoord;
} fs_in;

uniform sampler2D tex0; // texture unit from C++
uniform vec4 u_diffuse_color = vec4(1.0f);

void main()
{
    FragColor = u_diffuse_color * texture(tex0, fs_in.texCoord);
}