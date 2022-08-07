#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "uthash.h" // source: https://troydhanson.github.io/uthash/userguide.html

#define LPAREN_TOK '('
#define GT_TOK '>'
#define LT_TOK '<'
#define RPAREN_TOK ')'
#define HASH_TOK '#'
#define CURLY_BRACKET_START_TOK '{'
#define CURLY_BRACKET_END_TOK '}'
#define SQUARE_BRACKET_START_TOK '['
#define SQUARE_BRACKET_END_TOK ']'
#define EQUAL_TOK '='
#define SEMICOLON_TOK ';'
#define ADD_TOK '+'
#define MINUS_TOK '-'
#define DOUBLE_QUOTE_TOK '"'
#define COMMA_TOK ','
#define DIVIDE_TOK '/'
#define MULTIPLY_TOK '*'
#define MODULO_TOK '%'
#define NOT_TOK '!'
#define AND_TOK '&'
#define OR_TOK '|'
#define XOR_TOK '^'
#define COMPLEMENT_TOK '~'

#define WHILE_TOK 256
#define INCLUDE_TOK 257
#define VOID_TOK 258
#define FOR_TOK 259
#define INT_TOK 260
#define FLOAT_TOK 261
#define LONG_TOK 262
#define CHAR_TOK 263
#define ELSE_TOK 264

#define INTCONST 350
#define HEADER 351
int nextIdToken = 352;

FILE *yyin;
FILE *yyout;

char *yytext;
const int YYTEXT_MAX_SIZE = 100;

int yylength;

struct token_map_struct
{
    const char *name; /* key */
    int tok;
    UT_hash_handle hh; /* makes this structure hashable */
};
struct token_map_struct *tokens = NULL;

char getSingleCharToken(char c)
{
    switch (c)
    {
    case LPAREN_TOK:
        return LPAREN_TOK;
    case GT_TOK:
        return GT_TOK;
    case LT_TOK:
        return LT_TOK;
    case RPAREN_TOK:
        return RPAREN_TOK;
    case HASH_TOK:
        return HASH_TOK;
    case CURLY_BRACKET_START_TOK:
        return CURLY_BRACKET_START_TOK;
    case CURLY_BRACKET_END_TOK:
        return CURLY_BRACKET_END_TOK;
    case SQUARE_BRACKET_START_TOK:
        return SQUARE_BRACKET_START_TOK;
    case SQUARE_BRACKET_END_TOK:
        return SQUARE_BRACKET_END_TOK;
    case EQUAL_TOK:
        return EQUAL_TOK;
    case SEMICOLON_TOK:
        return SEMICOLON_TOK;
    case ADD_TOK:
        return ADD_TOK;
    case MINUS_TOK:
        return MINUS_TOK;
    case DOUBLE_QUOTE_TOK:
        return DOUBLE_QUOTE_TOK;
    case COMMA_TOK:
        return COMMA_TOK;
    case DIVIDE_TOK:
        return DIVIDE_TOK;
    case MULTIPLY_TOK:
        return MULTIPLY_TOK;
    case MODULO_TOK:
        return MODULO_TOK;
    case NOT_TOK:
        return NOT_TOK;
    case AND_TOK:
        return AND_TOK;
    case OR_TOK:
        return OR_TOK;
    case XOR_TOK:
        return XOR_TOK;
    case COMPLEMENT_TOK:
        return COMPLEMENT_TOK;
    default:
        return '0';
    }
}

bool isDelimiter(char ch)
{
    if (ch == ' ' || ch == '+' || ch == '-' || ch == '*' ||
        ch == '/' || ch == ',' || ch == ';' || ch == '>' ||
        ch == '<' || ch == '=' || ch == '(' || ch == ')' ||
        ch == '[' || ch == ']' || ch == '{' || ch == '}' || ch == '"')
        return true;
    return false;
}

void updateToSingleTokenFromMultipleToken()
{
    for (int i = 0; i < yylength; i++)
    {
        char ch = yytext[i];
        if (isDelimiter(ch))
        {
            if (fseek(yyin, -(yylength - i), SEEK_CUR))
                perror("fseek error");
            yylength = i;
            yytext[yylength] = '\0';
            break;
        }
    }
}

bool isDigit(char c)
{
    return (
        ((c - '0') >= 0) &&
        ((c - '0') <= 9));
}

bool isConstant()
{
    for (int i = 0; i < yylength; i++)
        if (!isDigit(yytext[i]))
            return false;

    return true;
}

bool isKeyword(char *keyword)
{
    return strcmp(yytext, keyword) == 0;
}

bool isHeader()
{
    if (yylength < 2)
        return false;
    char extension[2];

    memcpy(extension, &yytext[yylength - 2], 2);

    return strcmp(extension, ".h") == 0;
}

int yylex()
{
    if (feof(yyin))
        return -1;

    fscanf(yyin, "%s", yytext);

    yylength = strlen(yytext);

    char singleChar = getSingleCharToken(yytext[0]);
    if (singleChar != '0')
    {
        yytext[1] = '\0';
        printf("%s\t", yytext);
        return singleChar;
    }

    updateToSingleTokenFromMultipleToken();

    int token = -1;

    if (isConstant())
        token = INTCONST;
    else if (isHeader())
        token = HEADER;
    else if (isKeyword("include"))
        token = INCLUDE_TOK;
    else if (isKeyword("void"))
        token = HEADER;
    else if (isKeyword("for"))
        token = FOR_TOK;
    else if (isKeyword("int"))
        token = FOR_TOK;
    else if (isKeyword("float"))
        token = FLOAT_TOK;
    else if (isKeyword("long"))
        token = LONG_TOK;
    else if (isKeyword("while"))
        token = WHILE_TOK;
    else if (isKeyword("char"))
        token = CHAR_TOK;
    else if (isKeyword("else"))
        token = ELSE_TOK;

    if (token != -1)
    {
        printf("%s\t", yytext);
        return token;
    }

    struct token_map_struct *SToken;
    HASH_FIND_STR(tokens, yytext, SToken);

    if (SToken) // i.e data is found
        return SToken->tok;

    // else data is not present hence we add it and return token
    SToken = (struct token_map_struct *)malloc(sizeof *SToken);
    SToken->name = yytext;
    SToken->tok = nextIdToken;
    nextIdToken++;
    HASH_ADD_KEYPTR(hh, tokens, SToken->name, strlen(SToken->name), SToken);
    return SToken->tok;
}

void yywrap()
{
    fclose(yyin);
    free(yytext);

    struct token_map_struct *s, *tmp;

    HASH_ITER(hh, tokens, s, tmp)
    {
        HASH_DEL(tokens, s);
        free(s);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("err: filename missing");
        exit(-1);
    }

    int token;
    yyin = fopen(argv[1], "r");

    if (yyin == NULL)
        perror("err: fopen() error");

    yytext = (char *)malloc(sizeof(char) * YYTEXT_MAX_SIZE);

    while (!feof(yyin))
    {
        token = yylex();
        printf("%d\n", token);
    }

    yywrap();
    return 0;
}