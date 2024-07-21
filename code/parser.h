static String expect_token_kind(List* tokens, I64* offset, TokenKind kind) {
  Token token = *(Token*) list_get(tokens, *offset);
  if (token.kind == kind) {
    (*offset)++;
    return token.text;
  } else {
    print(string(ERROR "Expected "));
    print(token_kind_to_string(kind));
    print(string(", but got "));
    print_token(token);
    print(string(".\n"));
    exit(EXIT_FAILURE);
  }  
}

static String expect_token(List* tokens, I64* offset, TokenKind kind, String text) {
  Token token = *(Token*) list_get(tokens, *offset);
  if (token.kind == kind && strings_equal(token.text, text)) {
    (*offset)++;
    return token.text;
  } else {
    print(string(ERROR "Expected "));
    print_token((Token) { kind, text });
    print(string(", but got "));
    print_token(token);
    print(string(".\n"));
    exit(EXIT_FAILURE);
  }
}

static U32 next_token_is(List* tokens, I64* offset, String text) {
  Token token = *(Token*) list_get(tokens, *offset);
  return strings_equal(token.text, text);
}

static U32 next_token_kind_is(List* tokens, I64* offset, TokenKind kind) {
  Token token = *(Token*) list_get(tokens, *offset);
  return token.kind == kind;
}

static Type parse_type(List* tokens, I64* offset) {
  expect_token(tokens, offset, TOKEN_IDENTIFIER, string("I32"));
  return TYPE_I32;
}

static Signature parse_signature(List* tokens, I64* offset) {
  String name = expect_token_kind(tokens, offset, TOKEN_IDENTIFIER);
  expect_token(tokens, offset, TOKEN_SYMBOL, string("("));

  List arguments = make_list(Argument);
  while (!next_token_is(tokens, offset, string(")"))) {
    Argument* argument = list_add(&arguments);
    argument->name     = expect_token_kind(tokens, offset, TOKEN_IDENTIFIER);
    argument->type     = parse_type(tokens, offset);
    
    if (!next_token_is(tokens, offset, string(")"))) {
      expect_token(tokens, offset, TOKEN_SYMBOL, string(","));
    }
  }
  expect_token(tokens, offset, TOKEN_SYMBOL, string(")"));

  Signature signature = {};
  signature.name      = name;
  signature.arguments = arguments;
  return signature;
}

static Expression parse_expression(List* tokens, I64* offset);

static List parse_expression_scope(List* tokens, I64* offset) {
  List statements = make_list(Expression);
  expect_token(tokens, offset, TOKEN_SYMBOL, string("{"));
  while (!next_token_is(tokens, offset, string("}"))) {
    Expression* statement = list_add(&statements);
    *statement            = parse_expression(tokens, offset);
    expect_token(tokens, offset, TOKEN_SYMBOL, string(";"));
  }
  expect_token(tokens, offset, TOKEN_SYMBOL, string("}"));
  return statements;
}

static Expression parse_expression(List* tokens, I64* offset) {
  Expression expression;
  if (next_token_kind_is(tokens, offset, TOKEN_IDENTIFIER)) {
    expression.kind       = EXPRESSION_IDENTIFIER;
    expression.identifier = expect_token_kind(tokens, offset, TOKEN_IDENTIFIER);
  } else if (next_token_kind_is(tokens, offset, TOKEN_INTEGER)) {
    expression.kind       = EXPRESSION_INTEGER;
    expression.identifier = expect_token_kind(tokens, offset, TOKEN_INTEGER);
  } else if (next_token_is(tokens, offset, string("{"))) {
    (*offset)++;
    expression.kind  = EXPRESSION_SCOPE;
    expression.scope = parse_expression_scope(tokens, offset);
  }

  if (next_token_is(tokens, offset, string("("))) {
    (*offset)++;

    List arguments = make_list(Expression);
    while (!next_token_is(tokens, offset, string(")"))) {
      Expression* argument = list_add(&arguments);
      *argument            = parse_expression(tokens, offset);
      
      if (!next_token_is(tokens, offset, string(")"))) {
	expect_token(tokens, offset, TOKEN_SYMBOL, string(","));
      }      
    }
    expect_token(tokens, offset, TOKEN_SYMBOL, string(")"));
    
    Call call;
    call.function  = malloc(sizeof(Expression));
    *call.function = expression;
    call.arguments = arguments;

    expression.kind = EXPRESSION_CALL;
    expression.call = call;
  }
  
  return expression;
}

static Program parse(List* tokens) {
  I64     offset    = 0;
  Program program   = {};
  program.externs   = make_list(Signature);
  program.functions = make_list(Function);

  while (!next_token_kind_is(tokens, &offset, TOKEN_END)) {
    if (next_token_is(tokens, &offset, string("extern"))) {
      offset++;
      Signature* signature = list_add(&program.externs);
      *signature           = parse_signature(tokens, &offset);
      expect_token(tokens, &offset, TOKEN_SYMBOL, string(";"));
    } else if (next_token_is(tokens, &offset, string("function"))) {
      offset++;
      Function* function  = list_add(&program.functions);
      function->signature = parse_signature(tokens, &offset);
      function->body      = parse_expression_scope(tokens, &offset);
    } else {
      print(string(ERROR "Expected extern or function, got "));
      print_token(*(Token*) list_get(tokens, offset));
      print(string(".\n"));
      exit(EXIT_FAILURE);
    }
  }

  return program;
}
