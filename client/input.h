struct input
{
    b32 LeftMouseButtonDown;
    b32 LeftMouseButtonPressed;
    b32 RightMouseButtonDown;
    b32 RightMouseButtonPressed;
    u32 MouseX;
    u32 MouseY;

    u64 FingerDownId;
    b32 FingerDown;
    f32 FingerX;
    f32 FingerY;
};