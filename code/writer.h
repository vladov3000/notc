typedef struct {
  I32 fd;
  U8* buffer;
  I64 buffered;
  I64 capacity;
} Writer;

static Writer make_writer(I32 fd, I64 capacity) {
  Writer writer   = {};
  writer.fd       = fd;
  writer.buffer   = malloc(capacity);
  writer.capacity = capacity;
  return writer;
}

static void writer_flush(Writer* writer) {
  if (writer->buffered > 0) {
    write(writer->fd, writer->buffer, writer->buffered);
    writer->buffered = 0;
  }
}

static void writer_write(Writer* writer, String message) {
  if (message.size > writer->capacity) {
    writer_flush(writer);
    write(writer->fd, message.data, message.size);
  } else {
    if (message.size > writer->capacity - writer->buffered) {
      writer_flush(writer);
    }
    memcpy(&writer->buffer[writer->buffered], message.data, message.size);
    writer->buffered += message.size;
  }
}

static void writer_write_char(Writer* writer, U8 c) {
  if (writer->buffered == writer->capacity) {
    writer_flush(writer);
  }
  writer->buffer[writer->buffered] = c;
  writer->buffered++;
}

static void writer_newline(Writer* writer) {
  writer_write_char(writer, '\n');
}
