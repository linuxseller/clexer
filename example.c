#include <stdio.h>
#include <stdlib.h>

#define CLEXER_IMPL
#include "lexer.h"

int main(int argc, char **argv){
    Lexer lexer;
    int err = get_lexer_from_file(argv[1], &lexer);
    if(err!=0){
        fprintf(stderr, "Error: error getting lexer from file\n");
        exit(1);
    }
    printf("%s\n", lexer.source);
    Token tk = lexer_next_token(&lexer);
    printf("HIII\n");
    while(tk.type!=CLEXER_TOKEN_EOF){
        printf("token: %s - '%.*s'\n", TOKEN_TO_STR[tk.type], tk.sv.size, tk.sv.data);
        tk=lexer_next_token(&lexer);
    }
    return 0;
}
