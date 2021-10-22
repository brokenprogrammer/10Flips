STN_INTERNAL void
InitializeRenderer(renderer *Renderer)
{
    Renderer->TextureShader = CreateShader();

    glGenVertexArrays(1, &Renderer->TextureVAO);
    glBindVertexArray(Renderer->TextureVAO);
    {
        glGenBuffers(1, &Renderer->TextureInstanceBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, Renderer->TextureInstanceBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Renderer->TextureInstanceData), 0, GL_DYNAMIC_DRAW);
        
        i32 Stride = RENDERER_BYTES_PER_JOB;
        u32 Offset = 0;
        {
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, Stride,
                          (void *)(sizeof(f32)*0));
            glVertexAttribDivisor(0, 1);

            Offset += 4;
        }
        {
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, Stride,
                          (void *)(sizeof(f32)*Offset));
            glVertexAttribDivisor(1, 1);

            Offset += 3;
        }
        {
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, Stride,
                          (void *)(sizeof(f32)*Offset));
            glVertexAttribDivisor(2, 1);

            Offset += 3;
        }
        {
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, Stride,
                          (void *)(sizeof(f32)*Offset));
            glVertexAttribDivisor(3, 1);

            Offset += 3;
        }
        {
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, Stride,
                          (void *)(sizeof(f32)*Offset));
            glVertexAttribDivisor(4, 1);

            Offset += 3;
        }
    }
    glBindVertexArray(0);
}

STN_INTERNAL void 
PushTexture(renderer *Renderer, texture *Texture,
            vector4 Source, vector4 Destination)
{
    vector3 p1 = { Destination.X, Destination.Y };
    vector3 p2 = { Destination.X, Destination.Y + Destination.Height };
    vector3 p3 = { Destination.X + Destination.Width, Destination.Y };
    vector3 p4 = { Destination.X + Destination.Width, Destination.Y + Destination.Height };

    GLubyte *Data = (Renderer->TextureInstanceData + Renderer->TextureInstanceDataAllocPosition);
    ((f32 *)Data)[0]  = Source.X;
    ((f32 *)Data)[1]  = Source.Y;
    ((f32 *)Data)[2]  = Source.Width;
    ((f32 *)Data)[3]  = Source.Height;
    ((f32 *)Data)[4]  = p1.X;
    ((f32 *)Data)[5]  = p1.Y;
    ((f32 *)Data)[6]  = p1.Z;
    ((f32 *)Data)[7]  = p2.X;
    ((f32 *)Data)[8]  = p2.Y;
    ((f32 *)Data)[9]  = p2.Z;
    ((f32 *)Data)[10] = p3.X;
    ((f32 *)Data)[11] = p3.Y;
    ((f32 *)Data)[12] = p3.Z;
    ((f32 *)Data)[13] = p4.X;
    ((f32 *)Data)[14] = p4.Y;
    ((f32 *)Data)[15] = p4.Z;

    Renderer->TextureInstanceDataAllocPosition += RENDERER_BYTES_PER_JOB;
    Renderer->PushedJobs++;
}

STN_INTERNAL void
PushTextureAngle(renderer *Renderer, texture *Texture,
            vector4 Source, vector4 Destination, vector2 Center, f32 Angle)
{
    f32 TempX = Destination.X;
    f32 TempY = Destination.Y;
    Destination.X = Destination.Width / 2 - Center.X;
    Destination.Y = Destination.Height / 2 - Center.Y;
    Center.X += TempX;
    Center.Y += TempY;

    vector4 p1 = { - Destination.Width / 2, Destination.Height / 2, 0.0f, 1.0f };
    vector4 p2 = { - Destination.Width / 2, - Destination.Height / 2, 0.0f, 1.0f };
    vector4 p3 = { Destination.Width / 2, Destination.Height / 2, 0.0f, 1.0f };
    vector4 p4 = { Destination.Width / 2, - Destination.Height / 2, 0.0f, 1.0f };

    matrix4 Transform = Matrix4InitDiagonal(1.0f);
    Transform = Matrix4MultiplyMatrix4(Transform, Matrix4TranslateVector3(Vector3Init(Center.X, Center.Y, 0.0f)));
    Transform = Matrix4MultiplyMatrix4(Transform, Matrix4Rotate(Angle, Vector3Init(0.0f, 0.0f, 1.0f)));

    p1 = Vector4MultiplyMatrix4(p1, Transform);
    p2 = Vector4MultiplyMatrix4(p2, Transform);
    p3 = Vector4MultiplyMatrix4(p3, Transform);
    p4 = Vector4MultiplyMatrix4(p4, Transform);

    GLubyte *Data = (Renderer->TextureInstanceData + Renderer->TextureInstanceDataAllocPosition);
    ((f32 *)Data)[0]  = Source.X;
    ((f32 *)Data)[1]  = Source.Y;
    ((f32 *)Data)[2]  = Source.Width;
    ((f32 *)Data)[3]  = Source.Height;
    ((f32 *)Data)[4]  = p1.X;
    ((f32 *)Data)[5]  = p1.Y;
    ((f32 *)Data)[6]  = p1.Z;
    ((f32 *)Data)[7]  = p2.X;
    ((f32 *)Data)[8]  = p2.Y;
    ((f32 *)Data)[9]  = p2.Z;
    ((f32 *)Data)[10] = p3.X;
    ((f32 *)Data)[11] = p3.Y;
    ((f32 *)Data)[12] = p3.Z;
    ((f32 *)Data)[13] = p4.X;
    ((f32 *)Data)[14] = p4.Y;
    ((f32 *)Data)[15] = p4.Z;

    Renderer->TextureInstanceDataAllocPosition += RENDERER_BYTES_PER_JOB;
    Renderer->PushedJobs++;
}

STN_INTERNAL void
BeginFrame(renderer *Renderer)
{
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    Renderer->PushedJobs = 0;
    Renderer->TextureInstanceDataAllocPosition = 0;

    glClear(GL_COLOR_BUFFER_BIT);
}

STN_INTERNAL void
EndFrame(renderer *Renderer, texture *Texture)
{
    matrix4 ViewProjection  = Matrix4Orthographic(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1.0f, 1.0f);

    for (u32 Index = 0; Index < Renderer->PushedJobs; ++Index)
    {
        {
            glBindBuffer(GL_ARRAY_BUFFER, Renderer->TextureInstanceBuffer);
            glBufferSubData(GL_ARRAY_BUFFER, 0, (sizeof(f32) * 16), 
                            Renderer->TextureInstanceData + (Index * (sizeof(f32) * 16)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        glBindVertexArray(Renderer->TextureVAO);
        glUseProgram(Renderer->TextureShader);
        {
            glUniformMatrix4fv(glGetUniformLocation(Renderer->TextureShader, "view_projection"), 1, GL_FALSE,
                               &ViewProjection.Elements[0][0]);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Texture->Id);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glUniform1i(glGetUniformLocation(Renderer->TextureShader, "tex"), 0);
            glUniform2f(glGetUniformLocation(Renderer->TextureShader, "tex_resolution"),
                        (f32)Texture->Width,
                        (f32)Texture->Height);
            
            GLint First = 0;
            GLsizei Count = 4;
            GLsizei InstanceCount = 1;
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, First, Count, InstanceCount);
        }
        glBindVertexArray(0);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}