#define RENDERER_BYTES_PER_JOB (sizeof(f32) * 16)

struct renderer
{
    GLuint TextureShader;

    u32 PushedJobs;
    texture *JobTextures[512];

    GLuint TextureVAO;
    GLuint TextureInstanceBuffer;
    u32 TextureInstanceDataAllocPosition;
    // NOTE(Oskar): Max textures * number of inputs
    GLubyte TextureInstanceData[32768 *  RENDERER_BYTES_PER_JOB];
};