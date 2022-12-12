#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;   // λ�ñ���������λ��ֵΪ 0 
layout(location = 1) in vec3 aColor; // ��ɫ����������λ��ֵΪ 1
layout(location = 2) in vec2 aTexCoord; // texture coord attribute poistion is 2

out vec3 ourColor; // ��Ƭ����ɫ�����һ����ɫ
out vec2 TexCoord;

uniform float xOffset;
uniform float yOffset;

void main() 
{
   gl_Position = vec4(aPos.x + xOffset, aPos.y + yOffset, aPos.z, 1.0);
   ourColor = aColor; // ��ourColor����Ϊ���ǴӶ�����������õ���������ɫ
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