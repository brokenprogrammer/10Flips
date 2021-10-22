/*
   stn_math.h - v1.1.0

   ----------------------------------------------------------------------------

   MIT License

   Copyright (c) 2021 Nullsson

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#ifndef STN_MATH_H
#define STN_MATH_H

#define STN_PI                              (3.1415926535897f)
#define STN_PIF                             3.1415926535897f
#define STN_ONE_OVER_SQUARE_ROOT_OF_TWO_PI  0.3989422804
#define STN_ONE_OVER_SQUARE_ROOT_OF_TWO_PIF 0.3989422804f
#define STN_EULERS_NUMBER                   2.7182818284590452353602874713527
#define STN_EULERS_NUMBERF                  2.7182818284590452353602874713527f

union vector2
{
    struct
    {
        f32 X;
        f32 Y;
    };

    struct
    {
        f32 Width;
        f32 Height;
    };

    f32 Elements[2];
};

union vector3
{
    struct
    {
        f32 X;
        f32 Y;
        f32 Z;
    };

    struct
    {
        f32 R;
        f32 G;
        f32 B;
    };

    f32 Elements[3];
};

union vector4
{
    struct
    {
        f32 X;
        f32 Y;
        union
        {
            struct
            {
                f32 Z;

                union
                {
                    f32 W;
                    f32 Radius;
                };
            };
            struct
            {
                f32 Width;
                f32 Height;
            };
        };
    };

    struct
    {
        vector3 XYZ;
        f32 _Unused;
    };

    struct
    {
        f32 R;
        f32 G;
        f32 B;
        f32 A;
    };

    f32 Elements[4];
};

struct matrix4
{
    f32 Elements[4][4];
};

inline f32
Sin(f32 Angle)
{
    f32 Result = sinf(Angle);

    return (Result);
}

inline f32
Cos(f32 Angle)
{
    f32 Result = cosf(Angle);

    return (Result);
}

inline f32
Tan(f32 Angle)
{
    f32 Result = tanf(Angle);

    return (Result);
}

inline f32
ATan2(f32 Y, f32 X)
{
    f32 Result = atan2f(Y, X);

    return (Result);
}

inline f32
Square(f32 Value)
{
    f32 Result = Value * Value;

    return (Result);
}

inline f32
SquareRoot(f32 Value)
{
    f32 Result = sqrtf(Value);

    return (Result);
}

inline f32
AbsoluteValue(f32 Value)
{
    f32 Result = fabsf(Value);

    return(Result);
}

inline f32
Lerp(f32 A, f32 B, f32 t)
{
    f32 Result = A + (t * (B - A));

    return (Result);
}

STN_INTERNAL f32
MaximumF32(f32 A, f32 B)
{
    return A > B ? A : B;
}

STN_INTERNAL vector2
Vector2Init(f32 X, f32 Y)
{
    vector2 Result = { X, Y };
    return (Result);
}

STN_INTERNAL vector2
Vector2Add(vector2 A, vector2 B)
{
    vector2 Result = {A.X + B.X, A.Y + B.Y };
    return (Result);
}

STN_INTERNAL vector2
Vector2Minus(vector2 A, vector2 B)
{
    vector2 Result = { A.X - B.X, A.Y - B.Y };
    return (Result);
}

STN_INTERNAL vector2
Vector2MultiplyF32(vector2 V, f32 F)
{
    V.X *= F;
    V.Y *= F;
    return (V);
}

STN_INTERNAL f32
Vector2Dot(vector2 A, vector2 B)
{
    f32 Result = A.X * B.X + A.Y * B.Y;
    return (Result);
}

STN_INTERNAL f32
Vector2LengthSquared(vector2 V)
{
    f32 Result = V.X * V.X + V.Y * V.Y;
    return (Result);
}

STN_INTERNAL f32
Vector2Length(vector2 V)
{
    return SquareRoot(Vector2LengthSquared(V));
}


STN_INTERNAL vector2
Vector2Normalize(vector2 V)
{
    f32 Length = SquareRoot(Vector2LengthSquared(V));
    V.X /= Length;
    V.Y /= Length;

    return (V);
}

STN_INTERNAL vector3
Vector3Init(f32 X, f32 Y, f32 Z)
{
    vector3 Result = { X, Y, Z };
    return (Result);
}

STN_INTERNAL vector3
Vector3Add(vector3 A, vector3 B)
{
    vector3 Result = {A.X + B.X, A.Y + B.Y, A.Z + B.Z };
    return (Result);
}

STN_INTERNAL vector3
Vector3Minus(vector3 A, vector3 B)
{
    vector3 Result = {A.X - B.X, A.Y - B.Y, A.Z - B.Z };
    return (Result);
}

STN_INTERNAL vector3
Vector3MultiplyF32(vector3 V, f32 F)
{
    V.X *= F;
    V.Y *= F;
    V.Z *= F;
    return (V);
}

STN_INTERNAL f32
Vector3LengthSquared(vector3 V)
{
    f32 Result = V.X*V.X + V.Y * V.Y + V.Z * V.Z;
    return (Result);
}

STN_INTERNAL f32
Vector3Length(vector3 V)
{
    return SquareRoot(Vector3LengthSquared(V));
}

STN_INTERNAL vector3
Vector3Normalize(vector3 V)
{
    f32 Length = Vector3Length(V);
    vector3 Result = { V.X / Length, V.Y / Length, V.Z / Length };
    return (Result);
}

STN_INTERNAL f32
Vector3Dot(vector3 A, vector3 B)
{
    f32 Result = A.X * B.X + A.Y * B.Y + A.Z * B.Z;
    return (Result);
}

STN_INTERNAL vector3
Vector3Cross(vector3 A, vector3 B)
{
    vector3 Result = 
    {
        A.Y * B.Z - A.Z * B.Y,
        A.Z * B.X - A.X * B.Z,
        A.X * B.Y - A.Y * B.X,
    };

    return (Result);
}

STN_INTERNAL f32
MinimumInVector3(vector3 V)
{
    f32 Minimum = V.X;
    if(V.Y < Minimum) { Minimum = V.Y; }
    if(V.Z < Minimum) { Minimum = V.Z; }
    return (Minimum);
}

STN_INTERNAL f32
MaximumInVector3(vector3 V)
{
    f32 Maximum = V.X;
    if(V.Y > Maximum) { Maximum = V.Y; }
    if(V.Z > Maximum) { Maximum = V.Z; }
    return (Maximum);
}

STN_INTERNAL f32
ClampF32(f32 V, f32 L, f32 H)
{
    return V < L ? L : V > H ? H : V;
}

STN_INTERNAL i32
ClampI32(i32 I, i32 L, i32 H)
{
    return I < L ? L : I > H ? H : I;
}

STN_INTERNAL vector4
Vector4Init(f32 X, f32 Y, f32 Z, f32 W)
{
    vector4 Result = { X, Y, Z, W };
    return (Result);
}

STN_INTERNAL vector4
Vector4AddVector4(vector4 A, vector4 B)
{
    vector4 Result = { A.X + B.X, A.Y + B.Y, A.Z + B.Z, A.W + B.W };
    return (Result);
}

STN_INTERNAL vector4
Vector4MinusVector4(vector4 A, vector4 B)
{
    vector4 Result = { A.X - B.X, A.Y - B.Y, A.Z - B.Z, A.W - B.W };
    return (Result);
}

STN_INTERNAL b32
Vector4RectangleHasPoint(vector4 V, vector2 P)
{
    return(P.X >= V.X && P.X <= V.X + V.Width &&
           P.Y >= V.Y && P.Y <= V.Y + V.Height);
}

STN_INTERNAL vector4
Vector4MultiplyF32(vector4 A, f32 F)
{
    vector4 Result = { A.X * F, A.Y * F, A.Z * F, A.W * F };
    return (Result);
}

STN_INTERNAL vector4
Vector4MultiplyVector4(vector4 A, vector4 B)
{
    vector4 Result = { A.X * B.X, A.Y * B.Y, A.Z * B.Z, A.W * B.W };
    return (Result);
}

STN_INTERNAL vector4
Vector4MultiplyMatrix4(vector4 V, matrix4 M)
{
    vector4 Result = {};
    
    for(int i = 0; i < 4; ++i)
    {
        Result.Elements[i] = (V.Elements[0]*M.Elements[0][i] +
                              V.Elements[1]*M.Elements[1][i] +
                              V.Elements[2]*M.Elements[2][i] +
                              V.Elements[3]*M.Elements[3][i]);
    }
    
    return (Result);
}

STN_INTERNAL matrix4
Matrix4InitDiagonal(f32 Diagonal)
{
    matrix4 Result = 
    {
        {
            { Diagonal },
            { 0.0f, Diagonal },
            { 0.0f, 0.0f, Diagonal },
            { 0.0f, 0.0f, 0.0f, Diagonal }
        }
    };

    return (Result);
}

STN_INTERNAL matrix4
Matrix4MultiplyMatrix4(matrix4 A, matrix4 B)
{
    matrix4 Result = {};

    for (int j = 0; j < 4; ++j)
    {
        for (int i = 0; i < 4; ++i)
        {
            Result.Elements[i][j] = (A.Elements[0][j]*B.Elements[i][0] +
                                     A.Elements[1][j]*B.Elements[i][1] +
                                     A.Elements[2][j]*B.Elements[i][2] +
                                     A.Elements[3][j]*B.Elements[i][3]);
        }
    }

    return (Result);
}

STN_INTERNAL matrix4
Matrix4MultiplyF32(matrix4 A, f32 B)
{
    for(int J = 0; J < 4; ++J)
    {
        for(int I = 0; I < 4; ++I)
        {
            A.Elements[I][J] *= B;
        }
    }
    
    return (A);
}

STN_INTERNAL matrix4
Matrix4TranslateVector3(vector3 Translation)
{
    matrix4 Result = Matrix4InitDiagonal(1.f);
    
    Result.Elements[3][0] = Translation.X;
    Result.Elements[3][1] = Translation.Y;
    Result.Elements[3][2] = Translation.Z;
    
    return (Result);
}

STN_INTERNAL matrix4
Matrix4Rotate(f32 Angle, vector3 Axis)
{
    matrix4 Result = Matrix4InitDiagonal(1.f);
    
    Axis = Vector3Normalize(Axis);
    
    f32 sin_theta = sinf(Angle);
    f32 cos_theta = cosf(Angle);
    f32 cos_value = 1.0f - cos_theta;
    
    Result.Elements[0][0] = (Axis.X * Axis.X * cos_value) + cos_theta;
    Result.Elements[0][1] = (Axis.X * Axis.Y * cos_value) + (Axis.Z * sin_theta);
    Result.Elements[0][2] = (Axis.X * Axis.Z * cos_value) - (Axis.Y * sin_theta);
    
    Result.Elements[1][0] = (Axis.Y * Axis.X * cos_value) - (Axis.Z * sin_theta);
    Result.Elements[1][1] = (Axis.Y * Axis.Y * cos_value) + cos_theta;
    Result.Elements[1][2] = (Axis.Y * Axis.Z * cos_value) + (Axis.X * sin_theta);
    
    Result.Elements[2][0] = (Axis.Z * Axis.X * cos_value) + (Axis.Y * sin_theta);
    Result.Elements[2][1] = (Axis.Z * Axis.Y * cos_value) - (Axis.X * sin_theta);
    Result.Elements[2][2] = (Axis.Z * Axis.Z * cos_value) + cos_theta;
    
    return (Result);
}

STN_INTERNAL matrix4
Matrix4Orthographic(f32 Left, f32 Right, f32 Bottom, f32 Top, f32 Near, f32 Far)
{
    matrix4 Result = {};

    Result.Elements[0][0] = 2.f / (Right - Left);
    Result.Elements[1][1] = 2.f / (Top - Bottom);
    Result.Elements[2][2] = -2.f / (Far - Near);
    Result.Elements[3][3] = 1.f;
    Result.Elements[3][0] = (Left + Right) / (Left - Right);
    Result.Elements[3][1] = (Bottom + Top) / (Bottom - Top);
    Result.Elements[3][2] = (Far + Near) / (Near - Far);

    return (Result);
}

STN_INTERNAL matrix4
Matrix4ScaleVector3(vector3 Scale)
{
    matrix4 Result = Matrix4InitDiagonal(1.0f);
    
    Result.Elements[0][0] = Scale.X;
    Result.Elements[1][1] = Scale.Y;
    Result.Elements[2][2] = Scale.Z;
    
    return (Result);
}

#endif // STN_MATH_H