enum {
  END,
  ADDASGN,
  AND,
  ASGN,
  BREAK,
  CHAR,
  CHARCONST,
  COMMA,
  CONTINUE,
  DECR,
  DIV,
  DIVASGN,
  ELSE,
  ENUM,
  EQ,
  FOR,
  GE,
  GT,
  IDENT,
  IF,
  INCR,
  INT,
  INTCONST,
  LBRACE,
  LBRACKET,
  LE,
  LPAREN,
  LT,
  MINUS,
  MOD,
  MODASGN,
  MUL,
  MULASGN,
  NE,
  NORETURN,
  NOT,
  OR,
  PLUS,
  RBRACE,
  RBRACKET,
  RETURN,
  RPAREN,
  SEMI,
  STRLIT,
  SUBASGN,
  VOID,
};

int Line;
int Peekc = '\n';

int Peektok;
int Peekarg;

enum {
  MAXSTRLITBUF = 16777216,
  MAXSTRLIT = 1048576,
};
char Strlitbuf[MAXSTRLITBUF];
int Strlitend[MAXSTRLIT];
int Nstrlit;

enum {
  MAXIDENTBUF = 16777216,
  MAXIDENT = 1048576,
};
char Identbuf[MAXIDENTBUF];
int Identend[MAXIDENT];
char Tok[MAXIDENT];
int Nident = 1;

enum {
  MAXNODE = MAXIDENT,
};
int Bit[CHAR_BIT];
int Critbit[MAXNODE];
int Child[MAXNODE * 2];
int Nnode;

noreturn void error(char msg[]) {
  eputs("line ");
  eputd(Line);
  eputs(": ");
  eputs(msg);
  eputc('\n');
  exit(EXIT_FAILURE);
}

noreturn void internalerror(char func[]) {
  eputs("internal error: ");
  eputs(func);
  eputc('\n');
  exit(EXIT_FAILURE);
}

noreturn void oom(char type[]) {
  eputs("out of memory: ");
  eputs(type);
  eputc('\n');
  exit(EXIT_FAILURE);
}

void nextc() {
  if (Peekc == EOF) {
    internalerror("nextc");
  }
  Line += Peekc == '\n';
  Peekc = getc();
  if (!(Peekc == EOF || isspace(Peekc) || isprint(Peekc))) {
    error("invalid source character");
  }
}

int matchc(int c) {
  if (Peekc != c) {
    return 0;
  }
  nextc();
  return 1;
}

int consumec() {
  int c = Peekc;
  nextc();
  return c;
}

int isodigit(int c) { return '0' <= c && c <= '7'; }

int getstrc() {
  if (Peekc == EOF) {
    error("unterminated string literal or character constant");
  }
  if (matchc('\\')) {
    if (Peekc == EOF) {
      error("unterminated string literal or character constant");
    }
    if (Peekc == '"' || Peekc == '\'' || Peekc == '\\') {
      return consumec();
    }
    if (isodigit(Peekc)) {
      int c = consumec() - '0';
      for (int i = 1; i < 3 && isodigit(Peekc); i++) {
        c = c * 8 + (consumec() - '0');
      }
      if (c > UCHAR_MAX) {
        error("octal escape sequence out of range");
      }
      return c;
    }
    if (matchc('n')) {
      return '\n';
    }
    if (matchc('r')) {
      return '\r';
    }
    if (matchc('t')) {
      return '\t';
    }
    error("escape sequence not supported");
  }
  if (!isprint(Peekc)) {
    error(
        "unescaped control character in string literal or character constant");
  }
  return consumec();
}

void initcritbit() {
  Bit[CHAR_BIT - 1] = 1;
  for (int i = CHAR_BIT - 2; i >= 0; i--) {
    Bit[i] = Bit[i + 1] * 2;
  }
  Child[1] = -1;
}

int identlen(int ident) { return Identend[ident] - Identend[ident - 1]; }

int identc(int ident, int i) {
  if (i >= identlen(ident)) {
    return 0;
  }
  return Identbuf[Identend[ident - 1] + i];
}

int identbit(int ident, int i) {
  return identc(ident, i / CHAR_BIT) / Bit[i % CHAR_BIT] % 2;
}

int findident() {
  int node = Child[1];
  for (; node >= 0;) {
    node = Child[node * 2 + identbit(Nident, Critbit[node])];
  }
  return -node;
}

int critbit(int c1, int c2) {
  for (int i = 0; i < CHAR_BIT; i++) {
    if (c1 / Bit[i] % 2 != c2 / Bit[i] % 2) {
      return i;
    }
  }
  internalerror("critbit");
}

void insertident(int critbit) {
  int childi = 1;
  int node;
  for (;;) {
    node = Child[childi];
    if (node < 0) {
      break;
    }
    if (!node) {
      internalerror("insertident");
    }
    int i = Critbit[node];
    if (i > critbit) {
      break;
    }
    if (i == critbit) {
      internalerror("insertident");
    }
    childi = node * 2 + identbit(Nident, i);
  }
  Nnode++;
  if (Nnode >= MAXNODE) {
    oom("node");
  }
  Critbit[Nnode] = critbit;
  int j = identbit(Nident, critbit);
  Child[Nnode * 2 + j] = -Nident;
  Child[Nnode * 2 + (1 - j)] = node;
  Child[childi] = Nnode;
}

int memoident(int tok) {
  int n = identlen(Nident);
  int ident = findident();
  int n2 = identlen(ident);
  if (n2 > n) {
    n = n2;
  }
  for (int i = 0; i < n; i++) {
    int c1 = identc(Nident, i);
    int c2 = identc(ident, i);
    if (c1 != c2) {
      Tok[Nident] = tok;
      insertident(i * CHAR_BIT + critbit(c1, c2));
      return Nident;
    }
  }
  Nident--;
  return ident;
}

int keyword(char s[], int tok) {
  int i = Identend[Nident];
  for (int j = 0; s[j]; j++) {
    Identbuf[i] = s[j];
    i++;
  }
  Nident++;
  Identend[Nident] = i;
  return memoident(tok);
}

int gettok() {
  for (;;) {
    for (; isspace(Peekc); nextc()) {
    }
    if (Peekc == EOF) {
      return END;
    }
    if (matchc('!')) {
      if (matchc('=')) {
        return NE;
      }
      return NOT;
    }
    if (matchc('"')) {
      int i = Strlitend[Nstrlit];
      for (;;) {
        if (i >= MAXSTRLITBUF) {
          oom("strlitbuf");
        }
        if (matchc('"')) {
          // terminating null character
          i++;
          break;
        }
        Strlitbuf[i] = getstrc();
        i++;
      }
      Nstrlit++;
      if (Nstrlit >= MAXSTRLIT) {
        oom("strlit");
      }
      Strlitend[Nstrlit] = i;
      Peekarg = Nstrlit;
      return STRLIT;
    }
    if (matchc('%')) {
      if (matchc('=')) {
        return MODASGN;
      }
      return MOD;
    }
    if (matchc('&')) {
      if (!matchc('&')) {
        error("bitwise AND / address-of not supported");
      }
      return AND;
    }
    if (matchc('\'')) {
      if (matchc('\'')) {
        error("empty character constant");
      }
      Peekarg = getstrc();
      if (!matchc('\'')) {
        getstrc();
        error("multi-character character constant");
      }
      return CHARCONST;
    }
    if (matchc('(')) {
      return LPAREN;
    }
    if (matchc(')')) {
      return RPAREN;
    }
    if (matchc('*')) {
      if (matchc('=')) {
        return MULASGN;
      }
      return MUL;
    }
    if (matchc('+')) {
      if (matchc('+')) {
        return INCR;
      }
      if (matchc('=')) {
        return ADDASGN;
      }
      return PLUS;
    }
    if (matchc(',')) {
      return COMMA;
    }
    if (matchc('-')) {
      if (matchc('-')) {
        return DECR;
      }
      if (matchc('=')) {
        return SUBASGN;
      }
      return MINUS;
    }
    if (matchc('/')) {
      if (matchc('/')) {
        for (; Peekc != EOF && !matchc('\n'); nextc()) {
        }
        continue;
      }
      if (matchc('=')) {
        return DIVASGN;
      }
      return DIV;
    }
    if (isdigit(Peekc)) {
      if (matchc('0')) {
        Peekarg = 0;
      } else {
        Peekarg = consumec() - '0';
        for (; isdigit(Peekc);) {
          int i = consumec() - '0';
          if (Peekarg > INT_MAX / 10 ||
              (Peekarg == INT_MAX / 10 && i > INT_MAX % 10)) {
            error("integer constant out of range");
          }
          Peekarg = Peekarg * 10 + i;
        }
      }
      if (isalnum(Peekc) || Peekc == '_') {
        error("invalid integer constant");
      }
      return INTCONST;
    }
    if (matchc(';')) {
      return SEMI;
    }
    if (matchc('<')) {
      if (matchc('<')) {
        error("left shift not supported");
      }
      if (matchc('=')) {
        return LE;
      }
      return LT;
    }
    if (matchc('=')) {
      if (matchc('=')) {
        return EQ;
      }
      return ASGN;
    }
    if (matchc('>')) {
      if (matchc('=')) {
        return GE;
      }
      if (matchc('>')) {
        error("right shift not supported");
      }
      return GT;
    }
    if (isalpha(Peekc) || Peekc == '_') {
      int i = Identend[Nident];
      for (;;) {
        if (i >= MAXIDENTBUF) {
          oom("identbuf");
        }
        Identbuf[i] = consumec();
        i++;
        if (!(isalnum(Peekc) || Peekc == '_')) {
          break;
        }
      }
      Nident++;
      if (Nident >= MAXIDENT) {
        oom("ident");
      }
      Identend[Nident] = i;
      Peekarg = memoident(IDENT);
      return Tok[Peekarg];
    }
    if (matchc('[')) {
      return LBRACKET;
    }
    if (matchc(']')) {
      return RBRACKET;
    }
    if (matchc('{')) {
      return LBRACE;
    }
    if (matchc('|')) {
      if (!matchc('|')) {
        error("bitwise OR not supported");
      }
      return OR;
    }
    if (matchc('}')) {
      return RBRACE;
    }
    error("operator not supported");
  }
}

void nexttok() { Peektok = gettok(); }

int matchtok(int tok) {
  int match = Peektok == tok;
  if (match) {
    nexttok();
  }
  return match;
}

void initlex() {
  initcritbit();
  keyword("break", BREAK);
  keyword("char", CHAR);
  keyword("continue", CONTINUE);
  keyword("else", ELSE);
  keyword("enum", ENUM);
  keyword("for", FOR);
  keyword("if", IF);
  keyword("int", INT);
  keyword("noreturn", NORETURN);
  keyword("return", RETURN);
  keyword("void", VOID);
  nextc();
  nexttok();
}
