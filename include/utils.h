#define MAX_STRING_SIZE 128

#ifndef UTILS_H_
#define UTILS_H_

#include <errno.h>
#include <string.h>
#include <math.h>

#define SAFE_OPEN_FILE(file_ptr, filename, mode)                                   \
                        file_ptr = fopen(filename, mode);                    \
                        if (file_ptr == NULL)                                      \
                        {                                                          \
                            printf("Unable open output file '%s': %s\n", filename, strerror(errno));    \
                            return ERROR_FILE;                                     \
                        }
#define SAFE_CALLOC(var_name, type, size) type *var_name = (type *) calloc(size, sizeof(type)); \
                                          if (var_name == NULL)                                 \
                                          {                                                     \
                                            return ERROR_CALLOC_IS_NULL;                        \
                                          }

// #define PRINT_ERROR(...) fprintf(stderr, __VA_ARGS__);

#define LOG(...) if (LOG_FILE != NULL) fprintf(LOG_FILE, __VA_ARGS__)

static const double EPSILON = 1e-5;
// header utils
typedef int err_code_t;

enum errors
{
    OK,
    ERROR_FILE,
    ERROR_TREE_IS_NULL,
    ERROR_TREE_LOOPED,
    ERROR_PARENT_DONT_MATCH,
    ERROR_CALLOC_IS_NULL,
    ERROR_CANT_GET_NEXT_NODE,
    ERROR_FILENAME_IS_EMPTY
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

static bool is_double_equal(double d1, double d2)
{
  if (fabs(d1 - d2) < EPSILON)
  {
      return true;
  }

  return false;
}

static bool is_integer(double num)
{
  return is_double_equal(num, (int) num);
}
#pragma GCC diagnostic pop

#endif //UTILS_H_
