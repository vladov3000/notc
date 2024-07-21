typedef enum {
  TYPE_UNIT,
  TYPE_I32,
} Type;

typedef struct {
  String name;
  Type   type;
} Argument;

typedef struct {
  String name;
  List   arguments;
  Type   result;
} Signature;

typedef struct Expression Expression;

typedef struct {
  Expression* function;
  List        arguments;
} Call;

typedef enum {
  EXPRESSION_IDENTIFIER,
  EXPRESSION_INTEGER,
  EXPRESSION_SCOPE,
  EXPRESSION_CALL,
} ExpressionKind;

struct Expression {
  ExpressionKind kind;
  union {
    String identifier;
    String integer;
    List   scope;
    Call   call;
  };
};

typedef struct {
  Signature signature;
  List      body;
} Function;

typedef struct {
  List externs;
  List functions;
} Program;
