static String prologue = string(
  "global _main\n"
);

static void emit_expression(Writer* writer, Expression* expression);

static void emit_expression_scope(Writer* writer, List* statements) {
  for (I64 i = 0; i < statements->size; i++) {
    Expression* statement = list_get(statements, i);
    emit_expression(writer, statement);
  }
}

static void emit_expression(Writer* writer, Expression* expression) {
  switch (expression->kind) {
  case EXPRESSION_IDENTIFIER:
    break;
    
  case EXPRESSION_INTEGER:
    writer_write(writer, string("  mov rax, "));
    writer_write(writer, expression->integer);
    writer_newline(writer);
    break;
    
  case EXPRESSION_SCOPE:
    break;
    
  case EXPRESSION_CALL: {
    Call* call      = &expression->call;
    List* arguments = &call->arguments;
    
    for (I64 i = 0; i < arguments->size; i++) {
      Expression* argument = list_get(arguments, i);
      emit_expression(writer, argument);
      writer_write(writer, string("  push rax\n"));
    }

    writer_write(writer, string("  pop rdi\n"));
    writer_write(writer, string("  call exit\n"));
    
    break;
  }
  }
}

static void emit(Writer* writer, Program* program) {
  writer_write(writer, prologue);
  
  List* externs   = &program->externs;
  List* functions = &program->functions;

  for (I64 i = 0; i < externs->size; i++) {
    Signature* extrn = list_get(externs, i);
    writer_write(writer, string("extern "));
    writer_write(writer, extrn->name);
    writer_newline(writer);
  }
  writer_newline(writer);

  for (I64 i = 0; i < functions->size; i++) {
    Function*  function  = list_get(functions, i);
    Signature* signature = &function->signature;
    List*      body      = &function->body;
    writer_write_char(writer, '_');
    writer_write(writer, signature->name);
    writer_write(writer, string(":\n"));
    emit_expression_scope(writer, body);
    writer_write(writer, string("  mov rax, 0\n"));
    writer_write(writer, string("  ret\n"));
    writer_newline(writer);
  }
  writer_newline(writer);
  
  writer_flush(writer);
  off_t written = lseek(writer->fd, 0, SEEK_CUR);
  ftruncate(writer->fd, written);

  if (system("nasm -f macho64 build/minimal.asm")) {
    print(string(ERROR "Failed to run assembler.\n"));
  }
  
  if (system("ld -L/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib -lSystem build/minimal.o -o build/minimal")) {
    print(string(ERROR "Failed to run linker.\n"));
  }
}
