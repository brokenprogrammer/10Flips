#ifndef MJSON_H
#define MJSON_H

#include <stdint.h>
#include <string.h> // TODO(Oskar): Work this dependency away.

#ifndef MJSON_NO_STATIC
#define MJSON_API static
#else
#define MJSON_API extern
#endif

enum mjson_type
{
    MJSON_UNDEFINED,
    MJSON_OBJECT,
    MJSON_ARRAY,
    MJSON_STRING,
    MJSON_PRIMITIVE
};

enum mjson_error
{
    MJSON_ERROR_NO_MEMORY = -1,
    MJSON_ERROR_INVALID_JSON = -2,
    MJSON_ERROR_INCOMPLETE_JSON = -3
};

struct mjson_token
{
    mjson_type Type;
    int Start;
    int End;
    int Size;
};

struct mjson_parser
{
    uint32_t Position;
    uint32_t TokenNext;
    int32_t TokenParent;
};

MJSON_API mjson_token *
mjson_init_token(mjson_parser *Parser, mjson_token *Tokens, uint32_t TokenLength)
{
    mjson_token *Token;
    
    if (Parser->TokenNext >= TokenLength) {
        return NULL;
    }

    Token = &Tokens[Parser->TokenNext++];
    Token->Start = Token->End = -1;
    Token->Size = 0;

    return Token;
}

MJSON_API void
mjson_set_token_data(mjson_token *Token, mjson_type Type, int Start, int End)
{
    Token->Type = Type;
    Token->Start = Start;
    Token->End = End;
    Token->Size = 0;
}

MJSON_API int 
mjson_parse_primitive(mjson_parser *Parser, char *Json, uint32_t JsonLength, mjson_token *Tokens, uint32_t TokenLength)
{
    mjson_token *Token;
    int Start = Parser->Position;

    for (; Parser->Position < JsonLength && Json[Parser->Position] != '\0'; ++Parser->Position)
    {
        switch(Json[Parser->Position])
        {
            case ':':
            case '\t':
            case '\r':
            case '\n':
            case ' ':
            case ',':
            case ']':
            case '}':
            {
                goto found;
            } break;
            default: 
            {
            } break;
        }

        if (Json[Parser->Position] < 32 || Json[Parser->Position] >= 127)
        {
            Parser->Position = Start;
            return MJSON_ERROR_INVALID_JSON;
        }
    }

    // NOTE(Oskar): Json primitive type has to be followed by a comma, object or array.
    Parser->Position = Start;
    return MJSON_ERROR_INVALID_JSON;

found:
    if (Tokens == NULL)
    {
        Parser->Position--;
        return 0;
    }

    Token = mjson_init_token(Parser, Tokens, TokenLength);
    if (Token == NULL)
    {
        Parser->Position = Start;
        return MJSON_ERROR_NO_MEMORY;
    }

    mjson_set_token_data(Token, MJSON_PRIMITIVE, Start, Parser->Position);

    Parser->Position--;
    return 0;
}

MJSON_API int
mjson_parse_string(mjson_parser *Parser, char *Json, uint32_t JsonLength, mjson_token *Tokens, uint32_t TokenLength)
{
    mjson_token *Token;
    int Start = Parser->Position;
    Parser->Position++;

    for (; Parser->Position < JsonLength && Json[Parser->Position] != '\0'; ++Parser->Position)
    {
        char C = Json[Parser->Position];

        // NOTE(Oskar): If quote then we found end of string.
        if (C == '\"')
        {
            if (Tokens == NULL)
            {
                return 0;
            }
            
            Token = mjson_init_token(Parser, Tokens, TokenLength);
            
            if (Token == NULL)
            {
                Parser->Position = Start;
                return MJSON_ERROR_NO_MEMORY;
            }
            
            mjson_set_token_data(Token, MJSON_STRING, Start + 1, Parser->Position);

            return 0;
        }

        // NOTE(Oskar): Parsing escaped characters and sequences.
        if (C == '\\' && Parser->Position + 1 < JsonLength) 
        {
            int Index;
            Parser->Position++;
            switch (Json[Parser->Position]) 
            {
                case '\"':
                case '/':
                case '\\':
                case 'b':
                case 'f':
                case 'r':
                case 'n':
                case 't':
                {

                } break;
                
                case 'u':
                {
                    // NOTE(Oskar): Parse hex character.
                    Parser->Position++;
                    for (Index = 0; Index < 4 && Parser->Position < JsonLength && Json[Parser->Position] != '\0'; Index++) {
                        if (!((Json[Parser->Position] >= 48 && Json[Parser->Position] <= 57) ||   /* 0-9 */
                            (Json[Parser->Position] >= 65 && Json[Parser->Position] <= 70) ||     /* A-F */
                            (Json[Parser->Position] >= 97 && Json[Parser->Position] <= 102)))     /* a-f */
                        {
                            Parser->Position = Start;
                            return MJSON_ERROR_INVALID_JSON;
                        }
                        Parser->Position++;
                    }
                    Parser->Position--;
                } break;
                
                default:
                {
                    Parser->Position = Start;
                    return MJSON_ERROR_INVALID_JSON;
                }
            }
        }
    }

    Parser->Position = Start;
    return MJSON_ERROR_INCOMPLETE_JSON;
}

MJSON_API int
mjson_parse(mjson_parser *Parser, char *Json, uint32_t JsonLength, mjson_token *Tokens, uint32_t TokenLength)
{
    int32_t Result = 0;
    int32_t Index = 0;
    mjson_token *CurrentToken;
    int32_t Count = Parser->TokenNext;

    for (; Parser->Position < JsonLength && Json[Parser->Position] != '\0'; ++Parser->Position)
    {
        char C = Json[Parser->Position];
        mjson_type Type;

        switch (C)
        {
            case '{':
            case '[':
            {
                Count++;
                if (Tokens == NULL)
                {
                    break;
                }

                CurrentToken = mjson_init_token(Parser, Tokens, TokenLength);
                if (CurrentToken == NULL)
                {
                    return MJSON_ERROR_NO_MEMORY;
                }

                if (Parser->TokenParent != -1)
                {
                    mjson_token *T = &Tokens[Parser->TokenParent];

                    if (T->Type == MJSON_OBJECT)
                    {
                        return MJSON_ERROR_INVALID_JSON;
                    }

                    T->Size++;
                }

                CurrentToken->Type = (C == '{' ? MJSON_OBJECT : MJSON_ARRAY);
                CurrentToken->Start = Parser->Position;
                Parser->TokenParent = Parser->TokenNext - 1;
            } break;

            case '}':
            case ']':
            {
                if (Tokens == NULL)
                {
                    break;
                }

                Type = (C == '}' ? MJSON_OBJECT : MJSON_ARRAY);

                for (Index = Parser->TokenNext - 1; Index >= 0; --Index)
                {
                    CurrentToken = &Tokens[Index];
                    if (CurrentToken->Start != -1 && CurrentToken->End == -1)
                    {
                        if (CurrentToken->Type != Type)
                        {
                            return MJSON_ERROR_INVALID_JSON;
                        }

                        Parser->TokenParent = -1;
                        CurrentToken->End = Parser->Position + 1;
                        break;
                    }
                }

                // NOTE(Oskar): Unmatched closing bracket is an error
                if (Index == -1)
                {
                    return MJSON_ERROR_INVALID_JSON;
                }

                for (; Index >= 0; --Index)
                {
                    CurrentToken = &Tokens[Index];
                    if (CurrentToken->Start != -1 && CurrentToken->End == -1)
                    {
                        Parser->TokenParent = Index;
                        break;
                    }
                }
            } break;

            case '\"':
            {
                Result = mjson_parse_string(Parser, Json, JsonLength, Tokens, TokenLength);
                if (Result < 0)
                {
                    return Result;
                }

                Count++;
                if (Parser->TokenParent != -1 && Tokens != NULL)
                {
                    Tokens[Parser->TokenParent].Size++;
                }
            } break;
            case '\t':
            case '\r':
            case '\n':
            case ' ':
            {
            } break;

            case ':':
            {
                Parser->TokenParent = Parser->TokenNext - 1;
            } break;

            case ',':
            {
                if (Tokens != NULL && Parser->TokenParent != -1 &&
                    Tokens[Parser->TokenParent].Type != MJSON_ARRAY &&
                    Tokens[Parser->TokenParent].Type != MJSON_OBJECT)
                {
                    for (Index = Parser->TokenNext - 1; Index >= 0; --Index)
                    {
                        if (Tokens[Index].Type == MJSON_ARRAY || Tokens[Index].Type == MJSON_OBJECT)
                        {
                            if (Tokens[Index].Start != -1 && Tokens[Index].End == -1)
                            {
                                Parser->TokenParent = Index;
                                break;
                            }
                        }
                    }
                }
            } break;

            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case 't':
            case 'f':
            case 'n':
            {
                if (Tokens != NULL && Parser->TokenParent != -1)
                {
                    mjson_token *T = &Tokens[Parser->TokenParent];
                    if (T->Type == MJSON_OBJECT || (T->Type == MJSON_STRING && T->Size != 0))
                    {
                        return MJSON_ERROR_INVALID_JSON;
                    }
                }

                Result = mjson_parse_primitive(Parser, Json, JsonLength, Tokens, TokenLength);
                if (Result < 0)
                {
                    return Result;
                }
                Count++;
                if (Parser->TokenParent != -1 && Tokens != NULL)
                {
                    Tokens[Parser->TokenParent].Size++;
                }
            } break;

            default:
            {
                return MJSON_ERROR_INVALID_JSON;
            }
        }
    }

    if (Tokens != NULL)
    {
        for (Index = Parser->TokenNext - 1; Index >= 0; --Index)
        {
            // NOTE(Oskar): Check for unmatched opening bracket.
            if (Tokens[Index].Start != -1 && Tokens[Index].End == -1)
            {
                return MJSON_ERROR_INCOMPLETE_JSON;
            }
        }
    }

    return Count;
}

MJSON_API int32_t
mjson_get_value(char *Key, char *Json, mjson_token *Tokens, uint32_t TokenLength)
{
    for (uint32_t Index = 0; Index < TokenLength; ++Index)
    {
        char *TokenValue = Json + Tokens[Index].Start;
        if (strncmp(TokenValue, Key, strlen(Key)) == 0)
        {
            return (Index+1);
        }
    }

    return (NULL);
}

MJSON_API void
mjson_init(mjson_parser *Parser)
{
    Parser->Position = 0;
    Parser->TokenNext = 0;
    Parser->TokenParent = -1;
}

#endif // MJSON_H