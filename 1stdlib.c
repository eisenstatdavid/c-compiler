// <ctype.h>

int isspace(int c) { return ('\t' <= c && c <= '\r') || c == ' '; }
int isprint(int c) { return ' ' <= c && c <= '~'; }
int isdigit(int c) { return '0' <= c && c <= '9'; }
int isalpha(int c) { return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z'); }
int isalnum(int c) { return isdigit(c) || isalpha(c); }

// <limits.h>

enum {
  CHAR_BIT = 8,
  UCHAR_MAX = 255,
  INT_MAX = 2147483647,
  INT_MIN = -INT_MAX - 1,
};

// <stdlib.h>

enum {
  EXIT_SUCCESS,
  EXIT_FAILURE,
};

// <stdio.h>

enum {
  BUFSIZ = 4096,
  EOF = -1,
};

char Errbuf[BUFSIZ];
int Nerrbuf;

void eflush() {
  for (int i = 0; i < Nerrbuf;) {
    int n = primwrite(2, Errbuf, i, Nerrbuf - i);
    if (n < 0) {
      prim_exit(EXIT_FAILURE);
    }
    i += n;
  }
  Nerrbuf = 0;
}

void eputc(int c) {
  if (Nerrbuf >= BUFSIZ) {
    eflush();
  }
  Errbuf[Nerrbuf] = c;
  Nerrbuf++;
  if (c == '\n') {
    eflush();
  }
}

void eputs(char s[]) {
  for (int i = 0; s[i]; i++) {
    eputc(s[i]);
  }
}

void eputnegd(int i) {
  int j = i / 10;
  if (j) {
    eputnegd(j);
  }
  eputc('0' - i % 10);
}

void eputd(int i) {
  if (i < 0) {
    eputc('-');
  } else {
    i = -i;
  }
  eputnegd(i);
}

void eputdln(int i) {
  eputd(i);
  eputc('\n');
}

noreturn void panic(char msg[], int err) {
  eputs("panic: ");
  eputs(msg);
  eputs(": ");
  eputdln(err);
  eflush();
  prim_exit(EXIT_FAILURE);
}

char Inbuf[BUFSIZ];
int Inbufi;
int Ninbuf;

int getc() {
  if (Inbufi < Ninbuf) {
    int c = Inbuf[Inbufi];
    Inbufi++;
    return c;
  }
  Ninbuf = primread(0, Inbuf, BUFSIZ);
  if (Ninbuf < 0) {
    panic("getc", Ninbuf);
  }
  if (!Ninbuf) {
    return EOF;
  }
  Inbufi = 1;
  return Inbuf[0];
}

char Outbuf[BUFSIZ];
int Noutbuf;

void flush() {
  for (int i = 0; i < Noutbuf;) {
    int n = primwrite(1, Outbuf, i, Noutbuf - i);
    if (n < 0) {
      panic("flush", n);
    }
    i += n;
  }
  Noutbuf = 0;
}

void putc(int c) {
  if (Noutbuf >= BUFSIZ) {
    flush();
  }
  Outbuf[Noutbuf] = c;
  Noutbuf++;
  if (c == '\n') {
    flush();
  }
}

void puts(char s[]) {
  for (int i = 0; s[i]; i++) {
    putc(s[i]);
  }
}

void putnegd(int i) {
  int j = i / 10;
  if (j) {
    putnegd(j);
  }
  putc('0' - i % 10);
}

void putd(int i) {
  if (i < 0) {
    putc('-');
  } else {
    i = -i;
  }
  putnegd(i);
}

void putdln(int i) {
  putd(i);
  putc('\n');
}

// <stdlib.h>

noreturn void exit(int status) {
  flush();
  eflush();
  prim_exit(status);
}

int main();

noreturn void start() { exit(main()); }
