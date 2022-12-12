#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;   // 位置变量的属性位置值为 0 
layout(location = 1) in vec3 aColor; // 颜色变量的属性位置值为 1
layout(location = 2) in vec2 aTexCoord; // texture coord attribute poistion is 2

out vec3 ourColor; // 向片段着色器输出一个颜色
out vec2 TexCoord;

uniform float xOffset;
uniform float yOffset;

void main() 
{
   gl_Position = vec4(aPos.x + xOffset, aPos.y + yOffset, aPos.z, 1.0);
   ourColor = aColor; // 将ourColor设置为我们从顶点数据那里得到的输入颜色
   TexCoord = aTexCoord;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixValue;

void main() 
{
   color = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mixValue);
};