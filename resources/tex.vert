#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 uP_m = mat4(1.0f);
uniform mat4 uM_m = mat4(1.0f);
uniform mat4 uV_m = mat4(1.0f);

out vec3 ourColor;
out vec2 texCoord;

void main()
{
    gl_Position = uP_m * uV_m * uM_m * vec4(aPos, 1.0f);
    //ourColor = aColor;
    texCoord = aTexCoord;
}