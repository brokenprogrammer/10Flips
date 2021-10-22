const char *VertexShaderSource = 
    "#version 300 es\n"
    "\n"
    "in vec4 Vert_Source;\n"
    "in vec3 Vert_00;\n"
    "in vec3 Vert_01;\n"
    "in vec3 Vert_10;\n"
    "in vec3 Vert_11;\n"
    "\n"
    "out vec2 frag_uv;\n"
    "out vec4 frag_source;\n"
    "\n"
    "uniform mat4 view_projection;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vec4 source = Vert_Source;\n"
    "    vec3 vertices[] = vec3[](Vert_00, Vert_01, Vert_10, Vert_11);\n"
    "    vec4 world_space = vec4(vertices[gl_VertexID], 1);\n"
    "    vec4 clip_space = view_projection * world_space;\n"
    "    \n"
    "    vec2 uvs[] = vec2[](vec2(0, 0), vec2(0, 1), vec2(1, 0), vec2(1, 1));\n"
    "    frag_uv = uvs[gl_VertexID];\n"
    "    frag_source = source;\n"
    "    \n"
    "    gl_Position = clip_space;\n"
    "}\n"
    "";

const char *FragmentShaderSource = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "\n"
    "in vec2 frag_uv;\n"
    "in vec4 frag_source;\n"
    "out vec4 color;\n"
    "uniform sampler2D tex;\n"
    "uniform vec2 tex_resolution;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vec2 uv_offset = frag_source.xy;\n"
    "    vec2 uv_range = frag_source.zw;\n"
    "    vec4 tint = vec4(1.0, 1.0, 1.0, 1.0);\n"
    "    vec2 scale = vec2(1.0, 1.0);\n"
    "    \n"
    "    vec2 pixel = (uv_offset + (frag_uv * uv_range));\n"
    "    vec2 sample_uv = floor(pixel) + vec2(0.5, 0.5);\n"
    "    \n"
    "    sample_uv.x += 1.0 - clamp((1.0 - fract(pixel.x)) * abs(scale.x), 0.0, 1.0);\n"
    "    sample_uv.y += 1.0 - clamp((1.0 - fract(pixel.y)) * abs(scale.y), 0.0, 1.0);\n"
    "    \n"
    "    color = texture(tex, sample_uv / tex_resolution);\n"
    "    \n"
    "    if(color.a > 0.0)\n"
    "    {\n"
    "        color.xyz /= color.a;\n"
    "        color *= tint;\n"
    "    }\n"
    "    else\n"
    "    {\n"
    "        discard;\n"
    "    }\n"
    "}\n"
    "";

GLuint CreateShader()
{
    int InfoLogLength = 0;
    GLint Result = GL_FALSE;
    
    GLuint VS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VS, 1, &VertexShaderSource, NULL);
    glCompileShader(VS);
    
    // NOTE(Oskar): Check for VS errors
    {
        glGetShaderiv(VS, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(VS, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 1)
        {
            char VertexShaderError[1024] = {};
            InfoLogLength = ClampI32(InfoLogLength, 0, sizeof(VertexShaderError) - 1);
            glGetShaderInfoLog(VS, InfoLogLength, 0, VertexShaderError);
            printf("GLError: %s\n", VertexShaderError);
        }
        else
        {
            printf("VS Compiled successfully!\n");
        }
    }

    GLuint FS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FS, 1, &FragmentShaderSource, NULL);
    glCompileShader(FS);

    // NOTE(Oskar): Check for FS errors
    {
        glGetShaderiv(FS, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(FS, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 1)
        {
            char FragmentShaderError[1024] = {};
            InfoLogLength = ClampI32(InfoLogLength, 0, sizeof(FragmentShaderError) - 1);
            glGetShaderInfoLog(FS, InfoLogLength, 0, FragmentShaderError);
            printf("GLError: %s\n", FragmentShaderError);
        }
        else
        {
            printf("FS Compiled successfully!\n");
        }
    }

    GLuint Program = glCreateProgram();
    glAttachShader(Program, VS);
    glAttachShader(Program, FS);

    // NOTE(Oskar): Specify inputs
    {
        // NOTE(Oskar): Input
        glBindAttribLocation(Program, 0, "Vert_Source");
        glBindAttribLocation(Program, 1, "Vert_00");
        glBindAttribLocation(Program, 2, "Vert_01");
        glBindAttribLocation(Program, 3, "Vert_10");
        glBindAttribLocation(Program, 4, "Vert_11");

        // NOTE(Oskar): If i stumble into probs
        // https://stackoverflow.com/questions/16576390/equivalent-opengl-es-2-0-method-to-void-glbindfragdatalocationgluint-program-g
    }

    // NOTE(Oskar): All good, link and clean up
    {
        glLinkProgram(Program);
        glDeleteShader(VS);
        glDeleteShader(FS);
    }


    glValidateProgram(Program);
    glUseProgram(Program);

    return Program;    
}