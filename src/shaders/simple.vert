#version 450

// layout location соответствует твоему C++ Vertex
layout(location = 0) in vec2 inPosition; // ожидаем NDC x,y в [-1,1] (или близко)
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 fragColor;

void main() {
    vec2 pos = inPosition;

    gl_Position = vec4(pos, 0.0, 1.0);
    fragColor = inColor;
}

