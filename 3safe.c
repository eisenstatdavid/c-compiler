int issafeadd(int x, int y) {
  if (y < 0) {
    return x >= INT_MIN - y;
  }
  return x <= INT_MAX - y;
}

int safeadd(int x, int y) {
  if (!issafeadd(x, y)) {
    error("safeadd");
  }
  return x + y;
}

int issafediv(int x, int y) {
  if (y == -1) {
    return x != INT_MIN;
  }
  return y;
}

int safediv(int x, int y) {
  if (!issafediv(x, y)) {
    error("safediv");
  }
  return x / y;
}

int safemod(int x, int y) {
  if (!issafediv(x, y)) {
    error("safemod");
  }
  return x % y;
}

int issafemul(int x, int y) {
  if (y < 0) {
    if (x < 0) {
      return x >= INT_MAX / y;
    }
    return y == -1 || x <= INT_MIN / y;
  }
  if (x < 0) {
    return !y || x >= INT_MIN / y;
  }
  return !y || x <= INT_MAX / y;
}

int safemul(int x, int y) {
  if (!issafemul(x, y)) {
    error("safemul");
  }
  return x * y;
}

int issafeneg(int x) { return x != INT_MIN; }

int safeneg(int x) {
  if (!issafeneg(x)) {
    error("safeneg");
  }
  return -x;
}

int issafesub(int x, int y) {
  if (y < 0) {
    return x <= INT_MAX + y;
  }
  return x >= INT_MIN + y;
}

int safesub(int x, int y) {
  if (!issafesub(x, y)) {
    error("safesub");
  }
  return x - y;
}
