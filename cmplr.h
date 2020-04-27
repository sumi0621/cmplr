#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**********************/
/* グローバル変数      */
/**********************/
// トークンの種類
typedef enum {
    TK_RESERVED, // 記号
    TK_IDENT,   // 識別子
    TK_NUM,      // 整数トークン
    TK_RETURN,   // リターントークン
    TK_IF,
    TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

// トークン型
struct Token {
    TokenKind kind;  // トークンの型
    Token *next;     // 次の入力トークン
    int val;         // kindがTK_NUMの場合、その数値
    char *str;       // トークン文字列
    int len;         // トークンの長さ
};

typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
    LVar *next;  // 次の変数かNULL
    char *name;  // 変数の名前
    int len;     // 名前の長さ
    int offset;  // RBPからのオフセット
};

LVar *locals;

// 抽象構文木のノードの種類
typedef enum {
    ND_ADD,     // +
    ND_SUB,     // -
    ND_MUL,     // *
    ND_DIV,     // /
    ND_ASSIGN,  // =
    ND_NUM,     // 整数
    ND_LE,      // <=
    ND_LT,      // <
    ND_EQUAL,   // ==
    ND_NEQUAL,  // !=
    ND_LVAR,    // ローカル変数
    ND_RETURN,  // return
    ND_IF,      // if文
    ND_ELSE,    // else
    ND_WHILE,   // while
    ND_TRUE,
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
    NodeKind kind;  // ノードの型
    Node *lhs;      // 左辺
    Node *rhs;      // 右辺
    int val;        // kindがND_NUMの場合のみ使う
    int offset;     // kindがND_LVARの場合のみ使う
};

// 現在着目しているトークン
Token *token;

Node *code[100];

// 入力プログラム
char *user_input;

/**********************/
/* グローバル関数      */
/**********************/
extern Node *expr();
extern void tokenize();
extern void gen(Node *node);
extern void program();
extern void error(char *fmt, ...);
