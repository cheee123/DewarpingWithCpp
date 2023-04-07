#version 330

// Positions/Coordinates
layout (location = 0) in vec3 aPos;

// Texture Coordinates
layout (location = 1) in vec2 aTex;

// Outputs the texture coordinates to the fragment shader
out vec2 texCoord;

uniform mat4 camMatrix;

void main()
{
	gl_Position = camMatrix * vec4(aPos, 1.0);
	texCoord = vec2(aTex.x,1.0-aTex.y);
}
