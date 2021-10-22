/*
   stn_string.h - v1.1.0

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

#ifndef STN_STRING_H
#define STN_STRING_H

struct string
{
    char *Data;
    u32 Length;
    u32 Size;
    u32 MaxSize;
    b32 IsMutable;
};

enum string_pattern_match_flag
{
    StringPatternMatchFlag_None = 0x00,
    StringPatternMatchFlag_MaySkip = 0x01,
    StringPatternMatchFlag_Restarted = 0x02,
};

STN_INTERNAL b32
MatchPattern(char *Pattern, char *String)
{
    b32 Result = false;
    u32 Flags = StringPatternMatchFlag_None;
    char *P = Pattern;
    char *S = String;
    
    while (*S)
    {
        if (*P == '*')
        {
            Flags = StringPatternMatchFlag_MaySkip;
            ++P;
        }
        else if (Flags == StringPatternMatchFlag_MaySkip)
        {
            Result = true;
            if (*S == *P)
            {
                Flags = StringPatternMatchFlag_None;
                ++P;
                ++S;
            }
            else
            {
                Flags = StringPatternMatchFlag_MaySkip;
                ++S;
            }
        }
        else if (*S != *P)
        {
            if (Flags == StringPatternMatchFlag_Restarted)
            {
                break;
            }
            
            Result = false;
            Flags = StringPatternMatchFlag_Restarted;
            P = Pattern;
        }
        else
        {
            Result = true;
            Flags = StringPatternMatchFlag_None;
            ++P;
            ++S;
        }
    }
    
    return (Result && (*P == 0));
}

inline u32
StringLength(char *String)
{
#ifdef STN_USE_SSE
    u32 Result = 0;
  
    __m128i Zeroes   = _mm_setzero_si128();
    __m128i *Pointer = (__m128i *)String;

    int Mask = 0;
    for (; ; Pointer++, Result += 16)
    {
        __m128i Data = _mm_loadu_si128(Pointer);
        __m128i Comparison = _mm_cmpeq_epi8(Data, Zeroes);

        if ((Mask = _mm_movemask_epi8(Comparison)) != 0)
        {
#if defined(STN_COMPILER_MSVC)
            unsigned long Position;
            _BitScanForward(&Position, Mask);
            Result += Position;
#elif defined(STN_COMPILER_GCC) || defined(STN_COMPILER_CLANG)
            Result += __builtin_ctz(Mask);
#endif
            break;
        }
    }

    return Result;
#else
    char *CharPointer;
    u64 *BigPointer;
    u64 BigWord;
    u64 HighMagic = 0x8080808080808080;
    u64 LowMagic = 0x0101010101010101;

    // NOTE(Oskar): Handle first few bytes "manually" to align memory to u64
    for (CharPointer = String; 
         ((u64) CharPointer & (sizeof(u64) - 1)) != 0;
         ++CharPointer)
    {
        if (*CharPointer == '\0')
        {
            return CharPointer - String;
        }
    }

    // NOTE(Oskar): Casting this to a u64 allows us to perform 64 bit memory reads.
    BigPointer = (u64 *)CharPointer;

    for (;;)
    {
        BigWord = *BigPointer++;

        // NOTE(Oskar): Has zero check
        // source: https://graphics.stanford.edu/~seander/bithacks.html#ZeroInWord
        if (((BigWord - LowMagic) & ~BigWord & HighMagic) != 0)
        {
            const char *cp = (const char *) (BigPointer - 1);
            if (cp[0] == 0)
                return cp - String;
            if (cp[1] == 0)
                return cp - String + 1;
            if (cp[2] == 0)
                return cp - String + 2;
            if (cp[3] == 0)
                return cp - String + 3;
            if (cp[4] == 0)
                return cp - String + 4;
            if (cp[5] == 0)
                return cp - String + 5;
            if (cp[6] == 0)
                return cp - String + 6;
            if (cp[7] == 0)
                return cp - String + 7;
        }
    }
#endif
}

STN_INTERNAL b32
StringsAreEqual(char *A, char *B)
{
    b32 Result = (A == B);
    
    if(A && B)
    {
        while(*A && *B && (*A == *B))
        {
            ++A;
            ++B;
        }
        
        Result = ((*A == 0) && (*B == 0));
    }

    return (Result);
}

STN_INTERNAL b32
StringContains(char *A, char *B)
{
    b32 Result = false;

    if (A && B)
    {
        while (*A != '\0')
        {
            if ((*A == *B) && StringsAreEqual(A, B))
            {
                Result = true;
                break;
            }
            A++;
        }
    }

    return (Result);
}

STN_INTERNAL b32
CharIsSpace(char C)
{
    return (C <= 32);
}

STN_INTERNAL b32
CharIsAlpha(char C)
{
    return ((C >= 'a' && C <= 'z') ||
            (C >= 'A' && C <= 'Z'));
}

STN_INTERNAL b32
CharIsDigit(char C)
{
    return (C >= '0' && C <= '9');
}

STN_INTERNAL b32
CStringMatchCaseSensitive(const char *String1, const char *String2)
{
    b32 Result = 1;

    if (String1 && String2)
    {
        for (u32 Index = 0;; ++Index)
        {
            if (String1[Index] != String2[Index])
            {
                Result = 0;
                break;
            }

            if (String1[Index] == 0 && String2[Index] == 0)
            {
                break;
            }
        }
    }
    else if (String1 || String2)
    {
        Result = 0;
    }

    return (Result);
}

// NOTE(Oskar): This does not verify that the destination buffer is of the correct size.
//              users of this function will have to verify themselves.
STN_INTERNAL void
CopyCStringToFixedSizeBuffer(char *Destination, u32 DestinationMax, char *Source)
{
    for(u32 Index = 0; Index < DestinationMax; ++Index)
    {
        Destination[Index] = Source[Index];
        if(Source[Index] == 0)
        {
            break;
        }
    }
    Destination[DestinationMax - 1] = 0;
}

STN_INTERNAL i32
GetFirstI32FromCString(char *String)
{
    i32 Result = 0;

    int LastDigit = -1;

    for (u32 Index = 0; String[Index]; ++Index)
    {
        if (CharIsDigit(String[Index]))
        {
            LastDigit = Index;
        }
        else if (LastDigit != -1)
        {
            break;
        }
    }

    if (LastDigit >= 0)
    {
        int DigitMultiplier = 1;

        for (int Index = LastDigit; Index >= 0; --Index)
        {
            if (CharIsDigit(String[Index]))
            {
                int Digit = String[Index] - '0';
                Result += DigitMultiplier * Digit;
                DigitMultiplier *= 10;
            }
            else
            {
                if (String[Index] == '-')
                {
                    Result *= -1;
                }
                break;
            }
        }
    }

    return (Result);
}

STN_INTERNAL u32
HashCString(char *String)
{
    u32 Hash = 5381;
    int C;
    while((C = *String++))
    {
        Hash = ((Hash << 5) + Hash) + C;
    }

    return (Hash);
}

#endif // STN_STRING_H
