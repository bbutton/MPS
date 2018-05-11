#ifndef TEST_MACROS_H
#define TEST_MACROS_H

#define BUFS_EQUAL(expected, actual, bufLength)\
{\
  result_.countCheck();\
  if (memcmp(expected, actual, bufLength) != 0)\
  {\
    result_.addFailure(Failure(name_, __FILE__, __LINE__, StringFrom(expected), StringFrom(actual)));\
    return;\
  }\
}

#endif
