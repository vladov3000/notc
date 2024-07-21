#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#include "basic.h"
#include "print.h"
#include "list.h"
#include "lexer.h"
#include "syntax.h"
#include "parser.h"
#include "checker.h"
#include "writer.h"
#include "emitter.h"

static void read_file_error(char* operation) {
  String error = string(strerror(errno));
  print(string(ERROR "Failed to "));
  print(string(operation));
  print(string(" file: "));
  print(error);
  print(string(".\n"));
  exit(EXIT_FAILURE);
}

static String read_file(char* path) {
  I32 fd = open(path, O_RDONLY);
  if (fd == -1) {
    read_file_error("open");
  }

  struct stat info;
  if (fstat(fd, &info) != 0) {
    read_file_error("fstat");
  }

  I64 size = info.st_size;
  U8* text = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (text == MAP_FAILED) {
    read_file_error("mmap");
  }
  
  return (String) { text, size };
}

int main(int argc, char** argv) {
  atexit(flush);

  if (argc != 3) {
    print(string(ERROR "Expected 2 arguments.\n"));
    print(string("Usage: notc SOURCE_PATH OUTPUT_PATH\n"));
    exit(EXIT_FAILURE);
  }

  char* source_path = argv[1];
  char* output_path = argv[2];

  String source = read_file(source_path);
  print(string(INFO "Dumping source:\n"));
  print(source);
  print_char('\n');

  List tokens = lex(source);
  print(string(INFO "Dumping tokens:\n"));
  for (I64 i = 0; i < tokens.size; i++) {
    print_token(*(Token*) list_get(&tokens, i));
    print_char('\n');
  }

  Program program = parse(&tokens);
  check(&program);

  I32 output_fd = open(output_path, O_WRONLY | O_CREAT, 0666);
  if (output_fd == -1) {
    char* error = strerror(errno);
    print(string(ERROR "Failed to open output file: "));
    print(string(error));
    print(string(".\n"));
    exit(EXIT_FAILURE);
  }

  Writer writer = make_writer(output_fd, 4096);
  emit(&writer, &program);
}
