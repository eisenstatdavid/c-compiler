#include <errno.h>
#include <stdnoreturn.h>
#include <unistd.h>

noreturn void prim_exit(int status) { _exit(status); }

int primread(int fildes, char buf[], int nbyte) {
  int n = (int)read(fildes, buf, (size_t)nbyte);
  if (n == -1) {
    return -errno;
  }
  return n;
}

int primwrite(int fildes, char buf[], int offset, int nbyte) {
  int n = (int)write(fildes, buf + offset, (size_t)nbyte);
  if (n == -1) {
    return -errno;
  }
  return n;
}
