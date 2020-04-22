#include "cmplr.h"

// エラー箇所を特定する
void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, ""); // pos個の空白を出力
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char *op) {
    if (   (token->kind != TK_RESERVED)
        || (strlen(op) != token->len)
        || (memcmp(token->str, op, token->len))) {

        return false;
    }
    token = token->next;
    return true;
}

Token *consume_ident() {
    Token *tk = NULL;
    if (token->kind != TK_IDENT) {
        return tk;
    }
    tk = calloc(1, sizeof(Token));
    tk = token;
    token = token->next;
    return tk;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める
// それ以外の場合にはエラーを報告する
void expect(char *op) {
    if (   (token->kind != TK_RESERVED)
        || (strlen(op) != token->len)
        || (memcmp(token->str, op, token->len))) {

        error_at(token->str, "'%s'ではありません", op);
    }
    token = token->next;
}

// 次のトークンが数値の場合、トークンを1強み勧めてその数値を返す。
// それ以外の場合にはエラーを報告する
int expect_number() {
    if (token->kind != TK_NUM) {
        error_at(token->str, "数ではありません");
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *primary() {
    // 次のトークンが"("なら"(" expr ")" のはず
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    Token *tok = consume_ident();
    if (tok) {
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;
        node->offset = (tok->str[0] - 'a' + 1) * 8;
        return node;
    }

    // そうでなければ数値のはず
    return new_node_num(expect_number());
}

Node *unary() {
    if (consume("+")) {
        return primary();
    }
    if (consume("-")) {
        return new_node(ND_SUB, new_node_num(0), primary());
    }
    return primary();
}

Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume("*")) {
            node = new_node(ND_MUL, node, unary());
        }
        else if (consume("/")) {
            node = new_node(ND_DIV, node, unary());
        }
        else {
            return node;
        }
    }
}

Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume("+")) {
            node = new_node(ND_ADD, node, mul());
        }
        else if (consume("-")) {
            node = new_node(ND_SUB, node, mul());
        }
        else {
            return node;
        }
    }
}

Node *relational() {
    Node *node = add();

    for (;;) {
        if (consume("<=")) {
            node = new_node(ND_LE, node, add());
        }
        else if (consume(">=")) {
            node = new_node(ND_LE, add(), node);
        }
        else if (consume("<")) {
            node = new_node(ND_LT, node, add());
        }
        else if (consume(">")) {
            node = new_node(ND_LT, add(), node);
        }
        else {
            return node;
        }
    }
}

Node *equality() {
    Node *node = relational();

    for (;;) {
        if (consume("==")) {
            node = new_node(ND_EQUAL, node, relational());
        }
        else if (consume("!=")) {
            node = new_node(ND_NEQUAL, node, relational());
        }
        else {
            return node;
        }
    }
}

Node *assign() {
    Node *node = equality();
    if (consume("=")) {
        node = new_node(ND_ASSIGN, node, assign());
    }
    return node;
}

Node *expr() {
    return assign();
}

Node *stmt() {
    Node *node = expr();
    expect(";");
    return node;
}

void program() {
    int i = 0;
    while (!at_eof()) {
        code[i++] = stmt();
    }
    code[i] = NULL;
    
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int str_len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = str_len;
    cur->next = tok;

    return tok;
}

// 入力文字列pをトークナイズしてそれを返す
void tokenize() {
    Token head;
    head.next = NULL;
    Token *cur = &head;
    char *p = user_input;

    while (*p) {
        // 空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (   ('a' <= *p)
            && ('z' >= *p)) {

            cur = new_token(TK_IDENT, cur, p, 1);
            p++;
            continue;
        }

        if (   (strlen(p) >= 2)
            && (   (strncmp(p, "==", 2) == 0)
                || (strncmp(p, "!=", 2) == 0)
                || (strncmp(p, ">=", 2) == 0)
                || (strncmp(p, "<=", 2) == 0))) {

            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (   (*p == '+')
            || (*p == '-')
            || (*p == '*')
            || (*p == '/')
            || (*p == '(')
            || (*p == ')')
            || (*p == '<')
            || (*p == '>')
            || (*p == ';')
            || (*p == '=')) {

            cur = new_token(TK_RESERVED, cur, p, 1);
            p++;
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error_at(cur->str, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p, 0);
    token = head.next;
}