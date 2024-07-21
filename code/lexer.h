typedef struct {
  I64 line;
  I64 column;
} Location;

typedef enum {
  TOKEN_END,
  TOKEN_SYMBOL,
  TOKEN_IDENTIFIER,
  TOKEN_INTEGER,
} TokenKind;

static String token_kind_to_string(TokenKind kind) {
  switch (kind) {
  case TOKEN_SYMBOL:     return string("symbol");
  case TOKEN_IDENTIFIER: return string("identifier");
  case TOKEN_INTEGER:    return string("integer");
  case TOKEN_END:        return string("end of file");
  }
}

typedef struct {
  TokenKind kind;
  String    text;
} Token;

static void print_token(Token token) {
  print(token_kind_to_string(token.kind));
  if (token.kind != TOKEN_END) {
    print_char(' ');
    print(token.text);
  }
}

static U32 is_symbol(U8 c) {
  return c == '(' || c == ')' || c == ';' || c == ',' || c == '{' || c == '}';
}

static U32 is_whitespace(U8 c) {
  return c == ' ' || c == '\t' || c == '\n';
}

static U32 is_letter(U8 c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

static U32 is_digit(U8 c) {
  return '0' <= c && c <= '9';
}

static List lex(String source) {
  List     tokens   = make_list(Token);
  Location location = { 1, 1 };

  I64 index = 0;
  while (index < source.size) {
    while (index < source.size && is_whitespace(source.data[index])) {
      if (source.data[index] == '\n') {
	location.line++;
	location.column = 0;
      }
      location.column++;
      index++;
    }

    if (index == source.size) {
      break;
    }

    I64 start = index;
    U8  c     = source.data[index];
    if (is_symbol(c)) {
      Token* token = list_add(&tokens);
      token->kind  = TOKEN_SYMBOL;
      token->text  = slice(source, index, 1);
      index++;
    } else if (is_letter(c)) {
      I64 start = index;
      while (index < source.size && is_letter(source.data[index]) || is_digit(source.data[index])) {
	index++;
      }
      
      Token* token = list_add(&tokens);
      token->kind  = TOKEN_IDENTIFIER;
      token->text  = slice(source, start, index - start);
    } else if (is_digit(c)) {
      I64 start = index;
      while (index < source.size && is_digit(source.data[index])) {
	index++;
      }
      
      Token* token = list_add(&tokens);
      token->kind  = TOKEN_INTEGER;
      token->text  = slice(source, start, index - start);
    } else {
      print(string(ERROR));
      print_int(location.line);
      print_char(':');
      print_int(location.column);
      print(string(": Unexpected character '"));
      print_char(c);
      print(string("'.\n"));
      exit(EXIT_FAILURE);
    }

    location.column += index - start;
  }

  Token* token = list_add(&tokens);
  token->kind  = TOKEN_END;
  return tokens;
}
