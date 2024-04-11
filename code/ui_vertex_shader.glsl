// Source code of vertex shader
#version 130

// Vertex buffer
in vec2 vertex;
in vec3 color;
in vec2 uv;

// Uniform (global) buffer
uniform mat4 transformation_matrix;
uniform mat4 view_matrix;

// Attributes forwarded to the fragment shader
out vec4 color_interp;
out vec2 uv_interp;
out vec4 vertex_pos;

void main()
{
    // Transform vertex
    vertex_pos = vec4(vertex, 0.0, 1.0);
    gl_Position = view_matrix * transformation_matrix * vertex_pos;
    
    // Pass attributes to fragment shader
    color_interp = vec4(1.0, 0.0, 0.0, 1.0);
    uv_interp = uv;
}
