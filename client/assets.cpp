texture
LoadTexture(char *Path)
{
    texture Texture = {};
    Texture.IsLoaded = false;

    SDL_Surface *Image = IMG_Load(Path);

    if(!Image)
    {
        printf("Failed to load image: %s\n", IMG_GetError());
        return (Texture);
    }
    
    int bitsPerPixel = Image->format->BitsPerPixel;
    printf("Image dimensions %dx%d, %d bits per pixel\n", Image->w, Image->h, bitsPerPixel);

    Texture.Width = Image->w;
    Texture.Height = Image->h;

    GLint format = -1;
    if (bitsPerPixel == 24)
        format = GL_RGB;
    else if (bitsPerPixel == 32)
        format = GL_RGBA;

    if (format != -1)
    {
        glGenTextures(1, &Texture.Id);
        glBindTexture(GL_TEXTURE_2D, Texture.Id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, Texture.Width, Texture.Height, 0, format,
                     GL_UNSIGNED_BYTE, Image->pixels);

        glBindTexture(GL_TEXTURE_2D, 0);
        Texture.IsLoaded = true;

        printf("Texture successfully loaded!\n");
    }

    SDL_FreeSurface(Image);

    return (Texture);
}