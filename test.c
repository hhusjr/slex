#include <stdio.h>
#include "slex.h"

enum token_class {
    EMPTY,
    CHAR,
    STRING,
    IDENTIFIER,
    NUMBER,
    LEFT_PAR,
    RIGHT_PAR,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACE,
    STAR,
    SELF_INCREMENT,
    ASSIGN,
    IS_EQUAL,
    LT,
    GT,
    ADD,
    SUB,
    MOD,
    DIV,
    NOT,
    PREPROCESSOR,
    COLON,
    COMMA,
    SEMICOLON,
    AND,
    OR,
    LAND,
    LOR,
    IS_NOT_EQUAL,
    ERROR,
    DOT,
    ASK,
};

void preprocessor_1(const char_t *lexeme, size_t len, unsigned int token_class, bool *more, struct scanner *scanner,
                    void *args) {
    struct scanner *result = (struct scanner *) args;
    switch (token_class) {
        case 0:
            scanner_append(result, lexeme[0]);
            break;

        case 3:
            while (scanner->current_input->ch != '*' || scanner->current_input->next->ch != '/') {
                scanner_next(scanner);
            }
            scanner_next(scanner);
            scanner_next(scanner);
            break;

        default:
            break;
    }
    result->current_input = result->input_begin->next;
}

void
tokenizer(const char_t *lexeme, size_t len, unsigned int token_class, bool *more, struct scanner *scanner, void *args) {
    switch (token_class) {
        case CHAR:
            if (lexeme[len - 2] == '\\' && (len < 3 || lexeme[len - 3] != '\\')) {
                scanner_prev(scanner);
                SLEX_MORE();
                break;
            }
            wprintf(L"<CHAR, %ls>\n", lexeme);
            break;

        case STRING:
            if (lexeme[len - 2] == '\\' && (len < 3 || lexeme[len - 3] != '\\')) {
                scanner_prev(scanner);
                SLEX_MORE();
                break;
            }
            wprintf(L"<STRING, %ls>\n", lexeme);
            break;

        case IDENTIFIER:
            wprintf(L"<IDENTIFIER, %ls>\n", lexeme);
            break;

        case NUMBER:
            wprintf(L"<NUMBER, %ls>\n", lexeme);
            break;

        case LEFT_PAR:
            wprintf(L"<LEFT_PAR, %ls>\n", lexeme);
            break;

        case RIGHT_PAR:
            wprintf(L"<RIGHT_PAR, %ls>\n", lexeme);
            break;

        case LEFT_BRACKET:
            wprintf(L"<LEFT_BRACKET, %ls>\n", lexeme);
            break;

        case RIGHT_BRACKET:
            wprintf(L"<RIGHT_BRACKET, %ls>\n", lexeme);
            break;

        case LEFT_BRACE:
            wprintf(L"<LEFT_BRACE, %ls>\n", lexeme);
            break;

        case RIGHT_BRACE:
            wprintf(L"<RIGHT_BRACE, %ls>\n", lexeme);
            break;

        case STAR:
            wprintf(L"<STAR, %ls>\n", lexeme);
            break;

        case SELF_INCREMENT:
            wprintf(L"<SELF_INCREMENT, %ls>\n", lexeme);
            break;

        case ASSIGN:
            wprintf(L"<ASSIGN, %ls>\n", lexeme);
            break;

        case IS_EQUAL:
            wprintf(L"<IS_EQUAL, %ls>\n", lexeme);
            break;

        case IS_NOT_EQUAL:
            wprintf(L"<IS_NOT_EQUAL, %ls>\n", lexeme);
            break;

        case LT:
            wprintf(L"<LT, %ls>\n", lexeme);
            break;

        case GT:
            wprintf(L"<GT, %ls>\n", lexeme);
            break;

        case AND:
            wprintf(L"<AND, %ls>\n", lexeme);
            break;

        case OR:
            wprintf(L"<OR, %ls>\n", lexeme);
            break;

        case LAND:
            wprintf(L"<LAND, %ls>\n", lexeme);
            break;

        case LOR:
            wprintf(L"<LOR, %ls>\n", lexeme);
            break;

        case ADD:
            wprintf(L"<ADD, %ls>\n", lexeme);
            break;

        case SUB:
            wprintf(L"<SUB, %ls>\n", lexeme);
            break;

        case MOD:
            wprintf(L"<MOD, %ls>\n", lexeme);
            break;

        case DIV:
            wprintf(L"<DIV, %ls>\n", lexeme);
            break;

        case NOT:
            wprintf(L"<NOT, %ls>\n", lexeme);
            break;

        case PREPROCESSOR:
            wprintf(L"<PREPROCESSOR, %ls>\n", lexeme);
            break;

        case COMMA:
            wprintf(L"<COMMA, %ls>\n", lexeme);
            break;

        case DOT:
            wprintf(L"<DOT, %ls>\n", lexeme);
            break;

        case ASK:
            wprintf(L"<ASK, %ls>\n", lexeme);
            break;

        case ERROR:
            wprintf(L"<ERROR, %ls>\n", lexeme);
            break;

        default:
            break;
    }
}

int main() {
    struct slex_item *preprocessor_1_regexps = NULL;
    regexp_body_add(0, L".", &preprocessor_1_regexps);
    regexp_body_add(1, L"\\\\\\n", &preprocessor_1_regexps);
    regexp_body_add(2, L"//[^\\n]*", &preprocessor_1_regexps);
    regexp_body_add(3, L"/\\*", &preprocessor_1_regexps);
    struct slex_config *p1_cfg = slex_compile(preprocessor_1_regexps);
    struct scanner *p1_charstream = load_scanner();
    slex(p1_cfg, load_scanner_from_file("../regexp.c", UTF8), preprocessor_1, p1_charstream);

    struct slex_item *tokenizer_regexps = NULL;
    regexp_decl_add(L"digit", L"[0-9]", &tokenizer_regexps);
    regexp_decl_add(L"alpha", L"[a-zA-Z_]", &tokenizer_regexps);
    regexp_decl_add(L"empty", L"[\\r\\n\\t ]", &tokenizer_regexps);
    regexp_body_add(EMPTY, L"{empty}", &tokenizer_regexps);
    regexp_body_add(IDENTIFIER, L"{alpha}({digit}|{alpha})*", &tokenizer_regexps);
    regexp_body_add(NUMBER, L"{digit}+", &tokenizer_regexps);
    regexp_body_add(CHAR, L"'[^']*'", &tokenizer_regexps);
    regexp_body_add(STRING, L"\"[^\"]*\"", &tokenizer_regexps);
    regexp_body_add(LEFT_PAR, L"\\(", &tokenizer_regexps);
    regexp_body_add(RIGHT_PAR, L"\\)", &tokenizer_regexps);
    regexp_body_add(LEFT_BRACE, L"\\{", &tokenizer_regexps);
    regexp_body_add(RIGHT_BRACE, L"\\}", &tokenizer_regexps);
    regexp_body_add(LEFT_BRACKET, L"\\[", &tokenizer_regexps);
    regexp_body_add(RIGHT_BRACKET, L"\\]", &tokenizer_regexps);
    regexp_body_add(STAR, L"\\*", &tokenizer_regexps);
    regexp_body_add(SELF_INCREMENT, L"\\+\\+", &tokenizer_regexps);
    regexp_body_add(ASSIGN, L"=", &tokenizer_regexps);
    regexp_body_add(IS_EQUAL, L"==", &tokenizer_regexps);
    regexp_body_add(IS_NOT_EQUAL, L"!=", &tokenizer_regexps);
    regexp_body_add(LT, L"<", &tokenizer_regexps);
    regexp_body_add(GT, L">", &tokenizer_regexps);
    regexp_body_add(ADD, L"\\+", &tokenizer_regexps);
    regexp_body_add(SUB, L"\\-", &tokenizer_regexps);
    regexp_body_add(NOT, L"!", &tokenizer_regexps);
    regexp_body_add(MOD, L"%", &tokenizer_regexps);
    regexp_body_add(DIV, L"/", &tokenizer_regexps);
    regexp_body_add(LOR, L"\\|\\|", &tokenizer_regexps);
    regexp_body_add(OR, L"\\|", &tokenizer_regexps);
    regexp_body_add(LAND, L"&&", &tokenizer_regexps);
    regexp_body_add(DOT, L"\\.", &tokenizer_regexps);
    regexp_body_add(AND, L"&", &tokenizer_regexps);
    regexp_body_add(ASK, L"\\?", &tokenizer_regexps);
    regexp_body_add(COMMA, L",", &tokenizer_regexps);
    regexp_body_add(COLON, L":", &tokenizer_regexps);
    regexp_body_add(SEMICOLON, L";", &tokenizer_regexps);
    regexp_body_add(PREPROCESSOR, L"#{alpha}+[^\\n]*\\n", &tokenizer_regexps);
    regexp_body_add(ERROR, L".", &tokenizer_regexps);
    struct slex_config *t = slex_compile(tokenizer_regexps);
    slex(t, p1_charstream, tokenizer, NULL);
    return 0;
}
