#version 330 core

layout(location = 0) in vec3 aPos;  // Vertex position attribute

uniform mat4 model;  // The model matrix for transformations

void main()
{
    gl_Position = model * vec4(aPos, 1.0);  // Apply model transformation
}
