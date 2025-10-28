#version 450
vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);
layout(location = 0) out vec4 fragColor;
void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}

//
//#version 450
//
//layout(location = 0) in vec4 fragColor; // из vertex shader
//layout(location = 0) out vec4 outColor; // в framebuffer
//
//void main() {
//    outColor = fragColor;
//}
//
