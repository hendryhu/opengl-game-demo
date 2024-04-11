// Source code of fragment shader
#version 130

#define HEALTH_BAR 1000

// Attributes passed from the vertex shader
in vec4 color_interp;
in vec2 uv_interp;
in vec4 vertex_pos;

// Texture sampler
uniform sampler2D onetex;

// Uniform
uniform int object_type;
uniform float health;

void main()
{
    // Sample texture
    //vec4 color = texture2D(onetex, uv_interp);
    //vec4 color = color_interp;

    vec4 color = vec4(0.0, 0.0, 1.0, 1.0);

    switch(object_type){
    case HEALTH_BAR:
        if (vertex_pos.x < (10 * health)){
            color = vec4(0.184, 0.49, 0.165, 1.0);
        } else {
            color = vec4(1.0, 0.0, 0.0, 1.0);
        }
        break;

    default:
        color = texture2D(onetex, uv_interp);
        break;
    }
    

    // Assign color to fragment
    gl_FragColor = vec4(color.r, color.g, color.b, color.a);

    // Check for transparency
    if(color.a < 1.0)
    {
         discard;
    }
}
