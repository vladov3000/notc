#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))

// Unsigned integer types.
typedef unsigned char U8;
typedef unsigned int  U32;
typedef unsigned long U64;

// Signed integer types.
typedef signed char I8;
typedef signed int  I32;
typedef signed long I64;

typedef struct {
  U8* data;
  I64 size;
} String;

#define string(s) (String) { (U8*) (s), strlen(s) }

static String slice(String string, I64 start, I64 size) {
  String result;
  result.data = &string.data[min(start, string.size)];
  result.size = min(size, string.size - start);
  return result;
}

static U32 strings_equal(String a, String b) {
  return a.size == b.size && memcmp(a.data, b.data, a.size) == 0;
}
