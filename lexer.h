#include <string.h>
#include <ctype.h>

#ifndef _LEXER_H
typedef struct {
    char *data;
    size_t size;
} SView;

enum TokenEnum {
    CLEXER_TOKEN_NAME,
    CLEXER_TOKEN_OCURLY,
    CLEXER_TOKEN_CCURLY,
    CLEXER_TOKEN_OPAREN,
    CLEXER_TOKEN_CPAREN,
    CLEXER_TOKEN_OSQUAR,
    CLEXER_TOKEN_CSQUAR,
    CLEXER_TOKEN_STR_LITERAL,
    CLEXER_TOKEN_SEMICOLON,
    CLEXER_TOKEN_RETURN_COLON,
    CLEXER_TOKEN_COMMA,
    CLEXER_TOKEN_NUMERIC,
    CLEXER_TOKEN_EQUAL_SIGN,
    CLEXER_TOKEN_OP_DIV,
    CLEXER_TOKEN_OP_PLUS,
    CLEXER_TOKEN_OP_MINUS,
    CLEXER_TOKEN_OP_MUL,
    CLEXER_TOKEN_OP_LESS,
    CLEXER_TOKEN_OP_GREATER,
    CLEXER_TOKEN_OP_NOT,
    CLEXER_TOKEN_DOT,
    CLEXER_TOKEN_EOF
};

char *TOKEN_TO_STR[] = {
    [CLEXER_TOKEN_NAME         ] = "TOKEN_NAME",
    [CLEXER_TOKEN_OCURLY       ] = "TOKEN_OCURLY",
    [CLEXER_TOKEN_CCURLY       ] = "TOKEN_CCURLY",
    [CLEXER_TOKEN_OPAREN       ] = "TOKEN_OPAREN",
    [CLEXER_TOKEN_CPAREN       ] = "TOKEN_CPAREN",
    [CLEXER_TOKEN_STR_LITERAL  ] = "TOKEN_STR_LITERAL ",
    [CLEXER_TOKEN_SEMICOLON    ] = "TOKEN_SEMICOLON ",
    [CLEXER_TOKEN_RETURN_COLON ] = "TOKEN_RETURN_COLON",
    [CLEXER_TOKEN_COMMA        ] = "TOKEN_COMMA ",
    [CLEXER_TOKEN_NUMERIC      ] = "TOKEN_NUMERIC ",
    [CLEXER_TOKEN_EQUAL_SIGN   ] = "TOKEN_EQUAL_SIGN",
    [CLEXER_TOKEN_OP_DIV       ] = "TOKEN_OP_DIV",
    [CLEXER_TOKEN_OP_PLUS      ] = "TOKEN_OP_PLUS ",
    [CLEXER_TOKEN_OP_MINUS     ] = "TOKEN_OP_MINUS",
    [CLEXER_TOKEN_OP_MUL       ] = "TOKEN_OP_MUL",
    [CLEXER_TOKEN_OP_LESS      ] = "TOKEN_OP_LESS ",
    [CLEXER_TOKEN_OP_GREATER   ] = "TOKEN_OP_GREATER",
    [CLEXER_TOKEN_OP_NOT       ] = "TOKEN_OP_NOT",
    [CLEXER_TOKEN_DOT          ] = "TOKEN_DOT"
};

typedef struct {
    char *file_name;
    char *source;
    size_t line;
    size_t pos;
    size_t bol;
} Lexer;

typedef struct {
    char *file_path;
    size_t row;
    size_t col;
} LexerLocation;

typedef struct {
    enum TokenEnum type;
    SView sv;
    LexerLocation loc;
} Token;

#define CLEXER_CURR this->source[this->pos]
#define SVCMP(sv, b) strncmp(b, sv.data, MAX(sv.size, strlen(b)))
#define MAX(a,b) (a>b)?a:b

void lexer_chop_char(Lexer *this);
void lexer_trim_left(Lexer *this);
void lexer_drop_line(Lexer *this);
Token lexer_next_token(Lexer *lexer);
int get_lexer_from_file(char *file_name, Lexer *lexer);

#ifdef CLEXER_IMPL
int get_lexer_from_file(char *file_name, Lexer *lexer){
    lexer->file_name = file_name;
    FILE *file = fopen(file_name, "r");
    if(file==NULL){
        return -1;
    }
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);
    lexer->source = malloc(fsize+1); // accounting for \0
    fread(lexer->source, fsize, 1, file);
    return 0;
}

void lexer_chop_char(Lexer *this){
    if(CLEXER_CURR=='\0'){
        return;
    }
    char x = CLEXER_CURR;
    this->pos++;
    if(x=='\n'){
        this->bol = this->pos;
        this->line++;
    }
}

void lexer_trim_left(Lexer *this){
    while(isspace(CLEXER_CURR) && CLEXER_CURR!='\0'){
      lexer_chop_char(this);
    }
}

void lexer_drop_line(Lexer *this) {
    while(CLEXER_CURR!='\n' && CLEXER_CURR!='\0') {
        /* printf("dropped: %c\n", CLEXER_CURR); */
        lexer_chop_char(this);
    }
    if (CLEXER_CURR!='\0') {
        lexer_chop_char(this);
    }
}

Token lexer_next_token(Lexer *this){
    lexer_trim_left(this);
    SView sv = {0};
    char first_char = CLEXER_CURR;
    lexer_trim_left(this);
    first_char = CLEXER_CURR;
    LexerLocation loc = {.col       = this->pos-this->bol+1,
                    .row       = this->line,
                    .file_path = this->file_name};

    sv.data = this->source+this->pos;
    size_t start = this->pos;
    enum TokenEnum token_type;
    if(isalpha(first_char)){
        while(CLEXER_CURR!='\0' && isalnum(CLEXER_CURR)) {
            printf("%c\n",CLEXER_CURR);
            lexer_chop_char(this);
        }
        sv.size = this->pos - start;
        token_type = CLEXER_TOKEN_NAME;
        return (Token){.loc=loc, .sv=sv, .type=token_type};
    }
    if(isdigit(first_char)){
        while(CLEXER_CURR!='\0' && isdigit(CLEXER_CURR)) {
            lexer_chop_char(this);
            sv.size = this->pos - start;
        }
            return (Token){.loc=loc, .sv=sv, .type=CLEXER_TOKEN_NUMERIC};
    }
    sv.size=1;
    switch(first_char){
        case '\0':
            return (Token){.type = CLEXER_TOKEN_EOF};
        case '{':
            token_type = CLEXER_TOKEN_OCURLY; break;
        case '}':
            token_type = CLEXER_TOKEN_CCURLY; break;
        case '(':
            token_type = CLEXER_TOKEN_OPAREN; break;
        case ')':
            token_type = CLEXER_TOKEN_CPAREN; break;
        case '[':
            token_type = CLEXER_TOKEN_OSQUAR; break;
        case ']':
            token_type = CLEXER_TOKEN_CSQUAR; break;
        case ';':
            token_type = CLEXER_TOKEN_SEMICOLON; break;
        case ':':
            token_type = CLEXER_TOKEN_RETURN_COLON; break;
        case ',':
            token_type = CLEXER_TOKEN_COMMA; break;
        case '.':
            token_type = CLEXER_TOKEN_DOT; break;
        case '=':
            token_type = CLEXER_TOKEN_EQUAL_SIGN; break;
        case '+':
            token_type = CLEXER_TOKEN_OP_PLUS; break;
        case '-':
            token_type = CLEXER_TOKEN_OP_MINUS; break;
        case '/':
            token_type = CLEXER_TOKEN_OP_DIV; break;
        case '*':
            token_type = CLEXER_TOKEN_OP_MUL; break;
        case '<':
            token_type = CLEXER_TOKEN_OP_LESS; break;
        case '>':
            token_type = CLEXER_TOKEN_OP_GREATER; break;
        case '!':
            token_type = CLEXER_TOKEN_OP_NOT; break;
        case '"': { // parsing string literal
                    lexer_chop_char(this);
                    token_type = CLEXER_TOKEN_STR_LITERAL;
                    size_t str_lit_len = strpbrk(this->source+this->pos ,"\"") - (this->source + this->pos);
                    sv.data = calloc(str_lit_len+1, 1);
                    for(int i=0; CLEXER_CURR != '"'; i++){
                        if(CLEXER_CURR=='\\'){
                            lexer_chop_char(this);
                            switch(CLEXER_CURR){
                                case 'n':
                                    sv.data[i]='\n'; break;
                                case '\\':
                                    sv.data[i]='\\'; break;
                                case 't':
                                    sv.data[i]='\t'; break;
                                case '"':
                                    sv.data[i]='\"'; break;
                            }
                            lexer_chop_char(this);
                            continue;
                        }
                        sv.data[i]=CLEXER_CURR;
                        lexer_chop_char(this);
                    }
                    sv.size = str_lit_len;
                    break;
                  } // token string literal
    }
    lexer_chop_char(this);
    return (Token){.loc=loc, .sv=sv, .type=token_type};
}
#endif //CLEXER_IMPL
#endif //_LEXER_H
