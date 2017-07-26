enum {
  NOEXPR,
  EXPRADD,
  EXPRADDASGN,
  EXPRAND,
  EXPRASGN,
  EXPRCALL,
  EXPRCONST,
  EXPRDECR,
  EXPRDEREF,
  EXPRDIV,
  EXPRDIVASGN,
  EXPREQ,
  EXPRGE,
  EXPRGLOBAL,
  EXPRGT,
  EXPRINCR,
  EXPRLE,
  EXPRLOCAL,
  EXPRLT,
  EXPRMOD,
  EXPRMODASGN,
  EXPRMUL,
  EXPRMULASGN,
  EXPRNE,
  EXPRNEG,
  EXPRNOT,
  EXPROR,
  EXPRPOS,
  EXPRSTRLIT,
  EXPRSUB,
  EXPRSUBASGN,
  EXPRSUBSCR,
};

enum {
  NOTYPE,
  TYPECHAR,
  TYPECHARARR,
  TYPECHARARRREF,
  TYPECHARREF,
  TYPEINT,
  TYPEINTARR,
  TYPEINTARRREF,
  TYPEINTREF,
  TYPENORETURN,
  TYPEVOID,
};

enum {
  STATEUNDECL,
  STATEDECLUNDEF,
  STATEDEF,
};

enum {
  MAXEXPR = 16777216,
};
char Exprkind[MAXEXPR];
int Exprtype[MAXEXPR];
int Exprarg1[MAXEXPR];
int Exprarg2[MAXEXPR];
int Prevfuncarg[MAXEXPR];
int Nexpr;

enum {
  MAXLOCAL = 16777216,
};
int Localident[MAXLOCAL];
int Localexpr[MAXLOCAL];

int Globalstate[MAXIDENT];
int Globalexpr[MAXIDENT];
int Startident;

enum {
  MAXFUNC = 1048576,
  MAXPARAM = MAXLOCAL,
};
char Rettype[MAXFUNC];
char Paramtype[MAXPARAM];
int Paramident[MAXPARAM];
int Paramlistend[MAXFUNC];
int Nfunc;

int Nlabel;

void consumecomma() {
  if (!matchtok(COMMA)) {
    error("expected ,");
  }
}

int peekident() {
  if (Peektok != IDENT) {
    error("expected identifier");
  }
  return Peekarg;
}

void consumelbrace() {
  if (!matchtok(LBRACE)) {
    error("expected {");
  }
}

void consumelparen() {
  if (!matchtok(LPAREN)) {
    error("expected (");
  }
}

void consumerbrace() {
  if (!matchtok(RBRACE)) {
    error("expected }");
  }
}

void consumerbracket() {
  if (!matchtok(RBRACKET)) {
    error("expected ]");
  }
}

void consumerparen() {
  if (!matchtok(RPAREN)) {
    error("expected )");
  }
}

void consumesemi() {
  if (!matchtok(SEMI)) {
    error("expected ;");
  }
}

int matchtype() {
  if (Peektok == CHAR) {
    return TYPECHAR;
  }
  if (Peektok == INT) {
    return TYPEINT;
  }
  if (Peektok == VOID) {
    return TYPEVOID;
  }
  return NOTYPE;
}

int consumetype() {
  int type = matchtype();
  if (!type) {
    error("expected type");
  }
  nexttok();
  return type;
}

int newunexpr(int kind, int type, int arg1) {
  Nexpr++;
  if (Nexpr >= MAXEXPR) {
    oom("expr");
  }
  Exprkind[Nexpr] = kind;
  Exprtype[Nexpr] = type;
  Exprarg1[Nexpr] = arg1;
  return Nexpr;
}

int derefint(int expr) {
  int type = Exprtype[expr];
  if (type == TYPECHARREF || type == TYPEINTREF) {
    return newunexpr(EXPRDEREF, TYPEINT, expr);
  }
  return expr;
}

int newbinexpr(int kind, int type, int arg1, int arg2) {
  int expr = newunexpr(kind, type, arg1);
  Exprarg2[expr] = arg2;
  return expr;
}

int parseorexpr(int nlocal);

int parseprimexpr(int nlocal) {
  if (Peektok == CHARCONST || Peektok == INTCONST) {
    int val = Peekarg;
    nexttok();
    return newunexpr(EXPRCONST, TYPEINT, val);
  }
  if (Peektok == IDENT) {
    int ident = Peekarg;
    for (int i = nlocal - 1; i >= 0; i--) {
      if (Localident[i] == ident) {
        nexttok();
        return Localexpr[i];
      }
    }
    if (!Globalstate[ident]) {
      error("undeclared identifier");
    }
    nexttok();
    return Globalexpr[ident];
  }
  if (matchtok(LPAREN)) {
    int expr = parseorexpr(nlocal);
    consumerparen();
    return expr;
  }
  if (Peektok == STRLIT) {
    int strlit = Peekarg;
    nexttok();
    return newunexpr(EXPRSTRLIT, TYPECHARARR, strlit);
  }
  error("expected expression");
}

int parsearglist(int nlocal, int func) {
  int lastfuncarg = 0;
  int begin = Paramlistend[func - 1];
  int n = Paramlistend[func] - begin;
  for (int i = 0; i < n; i++) {
    if (Peektok == RPAREN) {
      error("too few arguments");
    }
    if (i) {
      consumecomma();
    }
    int expr = derefint(parseorexpr(nlocal));
    if (Exprtype[expr] != Paramtype[begin + i]) {
      error("argument type mismatch");
    }
    Prevfuncarg[expr] = lastfuncarg;
    lastfuncarg = expr;
  }
  if (Peektok == COMMA) {
    error("too many arguments");
  }
  return lastfuncarg;
}

int parsepostexpr(int nlocal) {
  int expr1 = parseprimexpr(nlocal);
  for (;;) {
    if (Peektok == LBRACKET) {
      int type;
      int arrtype = Exprtype[expr1];
      if (arrtype == TYPECHARARR) {
        type = TYPECHARREF;
      } else if (arrtype == TYPEINTARR) {
        type = TYPEINTREF;
      } else {
        error("left subscript operand can only have array type");
      }
      nexttok();
      int expr2 = derefint(parseorexpr(nlocal));
      if (Exprtype[expr2] != TYPEINT) {
        error("right subscript operand can only have integer type");
      }
      expr1 = newbinexpr(EXPRSUBSCR, type, expr1, expr2);
      consumerbracket();
    } else if (Peektok == LPAREN) {
      int type = Exprtype[expr1];
      if (type >= 0) {
        error("only functions can be called");
      }
      nexttok();
      int func = -type;
      int lastfuncarg = parsearglist(nlocal, func);
      consumerparen();
      expr1 = newbinexpr(EXPRCALL, Rettype[func], expr1, lastfuncarg);
    } else {
      return expr1;
    }
  }
}

int parseunexpr(int nlocal) {
  int kind;
  if (matchtok(MINUS)) {
    kind = EXPRNEG;
  } else if (matchtok(NOT)) {
    kind = EXPRNOT;
  } else if (matchtok(PLUS)) {
    kind = EXPRPOS;
  } else {
    return parsepostexpr(nlocal);
  }
  int expr = derefint(parseunexpr(nlocal));
  if (Exprtype[expr] != TYPEINT) {
    error("unary operand can only have integer type");
  }
  return newunexpr(kind, TYPEINT, expr);
}

int parsemulexpr(int nlocal) {
  int expr1 = parseunexpr(nlocal);
  for (;;) {
    int kind;
    if (Peektok == DIV) {
      kind = EXPRDIV;
    } else if (Peektok == MOD) {
      kind = EXPRMOD;
    } else if (Peektok == MUL) {
      kind = EXPRMUL;
    } else {
      return expr1;
    }
    expr1 = derefint(expr1);
    if (Exprtype[expr1] != TYPEINT) {
      error("left multiplicative operand can only have integer type");
    }
    nexttok();
    int expr2 = derefint(parseunexpr(nlocal));
    if (Exprtype[expr2] != TYPEINT) {
      error("right multiplicative operand can only have integer type");
    }
    expr1 = newbinexpr(kind, TYPEINT, expr1, expr2);
  }
}

int parseaddexpr(int nlocal) {
  int expr1 = parsemulexpr(nlocal);
  for (;;) {
    int kind;
    if (Peektok == MINUS) {
      kind = EXPRSUB;
    } else if (Peektok == PLUS) {
      kind = EXPRADD;
    } else {
      return expr1;
    }
    expr1 = derefint(expr1);
    if (Exprtype[expr1] != TYPEINT) {
      error("left additive operand can only have integer type");
    }
    nexttok();
    int expr2 = derefint(parsemulexpr(nlocal));
    if (Exprtype[expr2] != TYPEINT) {
      error("right additive operand can only have integer type");
    }
    expr1 = newbinexpr(kind, TYPEINT, expr1, expr2);
  }
}

int parserelexpr(int nlocal) {
  int expr1 = parseaddexpr(nlocal);
  int kind;
  if (Peektok == EQ) {
    kind = EXPREQ;
  } else if (Peektok == GE) {
    kind = EXPRGE;
  } else if (Peektok == GT) {
    kind = EXPRGT;
  } else if (Peektok == LE) {
    kind = EXPRLE;
  } else if (Peektok == LT) {
    kind = EXPRLT;
  } else if (Peektok == NE) {
    kind = EXPRNE;
  } else {
    return expr1;
  }
  expr1 = derefint(expr1);
  if (Exprtype[expr1] != TYPEINT) {
    error("left relational operand can only have integer type");
  }
  nexttok();
  int expr2 = derefint(parseaddexpr(nlocal));
  if (Exprtype[expr2] != TYPEINT) {
    error("right relational operand can only have integer type");
  }
  return newbinexpr(kind, TYPEINT, expr1, expr2);
}

int parseandexpr(int nlocal) {
  int expr1 = parserelexpr(nlocal);
  for (; Peektok == AND;) {
    expr1 = derefint(expr1);
    if (Exprtype[expr1] != TYPEINT) {
      error("left operand of && can only have integer type");
    }
    nexttok();
    int expr2 = derefint(parserelexpr(nlocal));
    if (Exprtype[expr2] != TYPEINT) {
      error("right operand of && can only have integer type");
    }
    expr1 = newbinexpr(EXPRAND, TYPEINT, expr1, expr2);
  }
  return expr1;
}

int parseorexpr(int nlocal) {
  int expr1 = parseandexpr(nlocal);
  for (; Peektok == OR;) {
    expr1 = derefint(expr1);
    if (Exprtype[expr1] != TYPEINT) {
      error("left operand of || can only have integer type");
    }
    nexttok();
    int expr2 = derefint(parseandexpr(nlocal));
    if (Exprtype[expr2] != TYPEINT) {
      error("right operand of || can only have integer type");
    }
    expr1 = newbinexpr(EXPROR, TYPEINT, expr1, expr2);
  }
  return expr1;
}

int parseexprstmt(int nlocal) {
  int expr1 = parseorexpr(nlocal);
  int kind;
  if (Peektok == ADDASGN) {
    kind = EXPRADDASGN;
  } else if (Peektok == ASGN) {
    kind = EXPRASGN;
  } else if (Peektok == DECR) {
    kind = EXPRDECR;
  } else if (Peektok == DIVASGN) {
    kind = EXPRDIVASGN;
  } else if (Peektok == INCR) {
    kind = EXPRINCR;
  } else if (Peektok == MODASGN) {
    kind = EXPRMODASGN;
  } else if (Peektok == MULASGN) {
    kind = EXPRMULASGN;
  } else if (Peektok == SUBASGN) {
    kind = EXPRSUBASGN;
  } else {
    return expr1;
  }
  int type = Exprtype[expr1];
  if (type != TYPECHARREF && type != TYPEINTREF) {
    error("left assignment operand can only be an lvalue of integer type");
  }
  nexttok();
  if (kind == EXPRDECR || kind == EXPRINCR) {
    return newunexpr(kind, TYPEINT, expr1);
  }
  int expr2 = derefint(parseorexpr(nlocal));
  if (Exprtype[expr2] != TYPEINT) {
    error("right assignment operand can only have integer type");
  }
  return newbinexpr(kind, TYPEINT, expr1, expr2);
}

int evalconstexpr(int expr) {
  int kind = Exprkind[expr];
  int arg1 = Exprarg1[expr];
  if (kind == EXPRCONST) {
    return arg1;
  }
  if (kind == EXPRDEREF) {
    return Strlitbuf[evalconstexpr(arg1)];
  }
  if (kind == EXPRNEG) {
    return safeneg(evalconstexpr(arg1));
  }
  if (kind == EXPRNOT) {
    return !evalconstexpr(arg1);
  }
  if (kind == EXPRPOS) {
    return evalconstexpr(arg1);
  }
  if (kind == EXPRSTRLIT) {
    return arg1;
  }
  int arg2 = Exprarg2[expr];
  if (kind == EXPRADD) {
    return safeadd(evalconstexpr(arg1), evalconstexpr(arg2));
  }
  if (kind == EXPRAND) {
    int val1 = evalconstexpr(arg1);
    int val2 = evalconstexpr(arg2);
    return val1 && val2;
  }
  if (kind == EXPRDIV) {
    return safediv(evalconstexpr(arg1), evalconstexpr(arg2));
  }
  if (kind == EXPREQ) {
    return evalconstexpr(arg1) == evalconstexpr(arg2);
  }
  if (kind == EXPRGE) {
    return evalconstexpr(arg1) >= evalconstexpr(arg2);
  }
  if (kind == EXPRGT) {
    return evalconstexpr(arg1) > evalconstexpr(arg2);
  }
  if (kind == EXPRLE) {
    return evalconstexpr(arg1) <= evalconstexpr(arg2);
  }
  if (kind == EXPRLT) {
    return evalconstexpr(arg1) < evalconstexpr(arg2);
  }
  if (kind == EXPRMOD) {
    return safemod(evalconstexpr(arg1), evalconstexpr(arg2));
  }
  if (kind == EXPRMUL) {
    return safemul(evalconstexpr(arg1), evalconstexpr(arg2));
  }
  if (kind == EXPRNE) {
    return evalconstexpr(arg1) != evalconstexpr(arg2);
  }
  if (kind == EXPROR) {
    int val1 = evalconstexpr(arg1);
    int val2 = evalconstexpr(arg2);
    return val1 || val2;
  }
  if (kind == EXPRSUB) {
    return safesub(evalconstexpr(arg1), evalconstexpr(arg2));
  }
  if (kind == EXPRSUBSCR) {
    int strlit = evalconstexpr(arg1);
    int begin = Strlitend[strlit - 1];
    int i = evalconstexpr(arg2);
    if (i < 0 || i >= Strlitend[strlit] - begin) {
      error("index out of range");
    }
    return begin + i;
  }
  error("expected constant expression");
}

void parseenumlist() {
  int val = -1;
  for (; Peektok != RBRACE;) {
    int ident = peekident();
    if (Globalstate[ident]) {
      error("identifier declared previously");
    }
    nexttok();
    if (matchtok(ASGN)) {
      int initexpr = derefint(parseorexpr(0));
      if (Exprtype[initexpr] != TYPEINT) {
        error("initializer can only have integer type");
      }
      val = evalconstexpr(initexpr);
    } else {
      val = safeadd(val, 1);
    }
    Globalexpr[ident] = newunexpr(EXPRCONST, TYPEINT, val);
    Globalstate[ident] = STATEDEF;
    if (!matchtok(COMMA)) {
      break;
    }
  }
}

void parseparamlist(int prevfunc, int rettype) {
  int prevbegin;
  int prevend;
  if (prevfunc) {
    if (rettype != Rettype[prevfunc]) {
      error("attribute or return type does not match previous declaration");
    }
    prevbegin = Paramlistend[prevfunc - 1];
    prevend = Paramlistend[prevfunc];
  }
  int begin = Paramlistend[Nfunc];
  int i = begin;
  if (Peektok != RPAREN) {
    for (;;) {
      int previ;
      if (prevfunc) {
        previ = prevbegin + (i - begin);
        if (previ >= prevend) {
          error("more parameters than previous declaration");
        }
      }
      int type = consumetype();
      int ident = peekident();
      for (int j = begin; j < i; j++) {
        if (Paramident[j] == ident) {
          error("duplicate parameter name");
        }
      }
      if (prevfunc && ident != Paramident[previ]) {
        error("parameter name does not match previous declaration");
      }
      nexttok();
      if (Peektok == LBRACKET) {
        if (type == TYPECHAR) {
          type = TYPECHARARR;
        } else if (type == TYPEINT) {
          type = TYPEINTARR;
        } else {
          error("array elements can only have integer type");
        }
        nexttok();
        consumerbracket();
      } else if (type != TYPEINT) {
        error("scalars can only have type int");
      }
      if (prevfunc && type != Paramtype[previ]) {
        error("parameter type does not match previous declaration");
      }
      if (i >= MAXPARAM) {
        oom("param");
      }
      Paramtype[i] = type;
      Paramident[i] = ident;
      i++;
      if (!matchtok(COMMA)) {
        break;
      }
    }
  }
  if (prevfunc && i - begin != prevend - prevbegin) {
    error("fewer parameters than previous declaration");
  }
  Nfunc++;
  if (Nfunc >= MAXFUNC) {
    oom("func");
  }
  Rettype[Nfunc] = rettype;
  Paramlistend[Nfunc] = i;
}

int newlabel() {
  Nlabel = safeadd(Nlabel, 1);
  return Nlabel;
}

void emitexpr(int expr);

void emitasgn(int expr1, char asgn[], int reg) {
  putc('\t');
  puts(asgn);
  puts(" [ebx], ");
  int type = Exprtype[expr1];
  if (type == TYPECHARREF) {
    putc(reg);
    putc('l');
  } else if (type == TYPEINTREF) {
    putc('e');
    putc(reg);
    putc('x');
  } else {
    internalerror("emitasgn");
  }
  putc('\n');
}

void emitbin(int expr1, int expr2) {
  emitexpr(expr2);
  puts("\tpush eax\n");
  emitexpr(expr1);
  puts("\tpop ecx\n");
}

void emitbool(char cond[]) {
  puts("\tmov ecx, eax\n");
  puts("\txor eax, eax\n");
  puts("\ttest ecx, ecx\n");
  puts("\tset");
  puts(cond);
  puts(" al\n");
}

void emitcondjmp(char cond[], int label) {
  puts("\ttest eax, eax\n");
  puts("\tj");
  puts(cond);
  puts(" label");
  putdln(label);
}

void emitincr(char incr[], int expr1) {
  emitexpr(expr1);
  putc('\t');
  puts(incr);
  putc(' ');
  int type = Exprtype[expr1];
  if (type == TYPECHARREF) {
    puts("byte");
  } else if (type == TYPEINTREF) {
    puts("dword");
  } else {
    internalerror("emitincr");
  }
  puts(" [ebx]\n");
}

void emitjmp(int label) {
  puts("\tjmp label");
  putdln(label);
}

void emitlabel(int label) {
  puts("label");
  putd(label);
  puts(":\n");
}

void emitlocal(char reg[], int offset) {
  puts("\tlea ");
  puts(reg);
  puts(", [ebp");
  offset *= 4;
  if (offset < 0) {
    puts(" - ");
    putd(-offset);
  } else if (offset) {
    puts(" + ");
    putd(offset);
  }
  puts("]\n");
}

void emitlog(int expr1, char cond[], int expr2) {
  emitexpr(expr1);
  int label = newlabel();
  emitcondjmp(cond, label);
  emitexpr(expr2);
  emitlabel(label);
  emitbool("nz");
}

void emitrel(int expr1, char rel[], int expr2) {
  emitbin(expr1, expr2);
  puts("\tmov edx, eax\n");
  puts("\txor eax, eax\n");
  puts("\tcmp edx, ecx\n");
  puts("\tset");
  puts(rel);
  puts(" al\n");
}

void emitexpr(int expr) {
  int kind = Exprkind[expr];
  int arg1 = Exprarg1[expr];
  int arg2 = Exprarg2[expr];
  if (kind == EXPRADD) {
    emitbin(arg1, arg2);
    puts("\tadd eax, ecx\n");
  } else if (kind == EXPRADDASGN) {
    emitbin(arg1, arg2);
    emitasgn(arg1, "add", 'c');
  } else if (kind == EXPRAND) {
    emitlog(arg1, "z", arg2);
  } else if (kind == EXPRASGN) {
    emitbin(arg1, arg2);
    emitasgn(arg1, "mov", 'c');
  } else if (kind == EXPRCALL) {
    int offset = 0;
    for (int funcarg = arg2; funcarg; funcarg = Prevfuncarg[funcarg]) {
      emitexpr(funcarg);
      puts("\tpush e");
      int type = Exprtype[funcarg];
      if (type == TYPECHARARR || type == TYPEINTARR) {
        putc('b');
      } else if (type == TYPEINT) {
        putc('a');
      } else {
        internalerror("emitexpr");
      }
      puts("x\n");
      offset += 4;
    }
    emitexpr(arg1);
    puts("\tcall ebx\n");
    if (Exprtype[expr] != TYPENORETURN && offset > 0) {
      puts("\tadd esp, ");
      putdln(offset);
    }
  } else if (kind == EXPRCONST) {
    puts("\tmov eax, ");
    putdln(arg1);
  } else if (kind == EXPRDECR) {
    emitincr("dec", arg1);
  } else if (kind == EXPRDEREF) {
    emitexpr(arg1);
    int type = Exprtype[arg1];
    if (type == TYPECHARREF) {
      puts("\tmovzx eax, byte [ebx]\n");
    } else if (type == TYPECHARARRREF || type == TYPEINTARRREF) {
      puts("\tmov ebx, [ebx]\n");
    } else if (type == TYPEINTREF) {
      puts("\tmov eax, [ebx]\n");
    } else {
      internalerror("emitexpr");
    }
  } else if (kind == EXPRDIV) {
    emitbin(arg1, arg2);
    puts("\tcdq\n");
    puts("\tidiv ecx\n");
  } else if (kind == EXPRDIVASGN) {
    emitbin(arg1, arg2);
    puts("\tmov eax, [ebx]\n");
    puts("\tcdq\n");
    puts("\tidiv ecx\n");
    emitasgn(arg1, "mov", 'a');
  } else if (kind == EXPREQ) {
    emitrel(arg1, "e", arg2);
  } else if (kind == EXPRGE) {
    emitrel(arg1, "nl", arg2);
  } else if (kind == EXPRGLOBAL) {
    puts("\tmov ebx, ident");
    putdln(arg1);
  } else if (kind == EXPRGT) {
    emitrel(arg1, "g", arg2);
  } else if (kind == EXPRINCR) {
    emitincr("inc", arg1);
  } else if (kind == EXPRLE) {
    emitrel(arg1, "ng", arg2);
  } else if (kind == EXPRLOCAL) {
    emitlocal("ebx", arg1);
  } else if (kind == EXPRLT) {
    emitrel(arg1, "l", arg2);
  } else if (kind == EXPRMOD) {
    emitbin(arg1, arg2);
    puts("\tcdq\n");
    puts("\tidiv ecx\n");
    puts("\tmov eax, edx\n");
  } else if (kind == EXPRMODASGN) {
    emitbin(arg1, arg2);
    puts("\tmov eax, [ebx]\n");
    puts("\tcdq\n");
    puts("\tidiv ecx\n");
    emitasgn(arg1, "mov", 'd');
  } else if (kind == EXPRMUL) {
    emitbin(arg1, arg2);
    puts("\timul ecx\n");
  } else if (kind == EXPRMULASGN) {
    emitbin(arg1, arg2);
    puts("\tmov eax, [ebx]\n");
    puts("\timul ecx\n");
    emitasgn(arg1, "mov", 'a');
  } else if (kind == EXPRNE) {
    emitrel(arg1, "ne", arg2);
  } else if (kind == EXPRNEG) {
    emitexpr(arg1);
    puts("\tneg eax\n");
  } else if (kind == EXPRNOT) {
    emitexpr(arg1);
    emitbool("z");
  } else if (kind == EXPROR) {
    emitlog(arg1, "nz", arg2);
  } else if (kind == EXPRPOS) {
    emitexpr(arg1);
  } else if (kind == EXPRSTRLIT) {
    puts("\tmov ebx, strlit");
    putdln(arg1);
  } else if (kind == EXPRSUB) {
    emitbin(arg1, arg2);
    puts("\tsub eax, ecx\n");
  } else if (kind == EXPRSUBASGN) {
    emitbin(arg1, arg2);
    emitasgn(arg1, "sub", 'c');
  } else if (kind == EXPRSUBSCR) {
    emitbin(arg1, arg2);
    puts("\tlea ebx, [ebx + ecx");
    int type = Exprtype[arg1];
    if (Exprtype[arg1] == TYPEINTARR) {
      puts("*4");
    } else if (type != TYPECHARARR) {
      internalerror("emitexpr");
    }
    puts("]\n");
  } else {
    internalerror("emitexpr");
  }
}

int parseandemitexprstmtorvardecl(int nlocalexblock, int nlocal) {
  int type = matchtype();
  if (!type) {
    emitexpr(parseexprstmt(nlocal));
    return nlocal;
  }
  if (type != TYPEINT) {
    error("scalars can only have type int");
  }
  nexttok();
  int ident = peekident();
  for (int i = nlocalexblock; i < nlocal; i++) {
    if (Localident[i] == ident) {
      error("variable declared previously in this block");
    }
  }
  nexttok();
  int init = matchtok(ASGN);
  if (init) {
    int initexpr = derefint(parseorexpr(nlocal));
    if (Exprtype[initexpr] != TYPEINT) {
      error("initializer can only have integer type");
    }
    emitexpr(initexpr);
  }
  if (nlocal >= MAXLOCAL) {
    oom("local");
  }
  Localident[nlocal] = ident;
  int nparam = Paramlistend[Nfunc] - Paramlistend[Nfunc - 1];
  int offset = -1 - (nlocal - nparam);
  Localexpr[nlocal] = newunexpr(EXPRLOCAL, TYPEINTREF, offset);
  nlocal++;
  emitlocal("esp", offset);
  if (init) {
    puts("\tmov [esp], eax\n");
  }
  return nlocal;
}

void prologue(int ident) {
  if (ident == Startident) {
    puts("_start:\n");
    puts("start:\n");
  } else {
    puts("ident");
    putd(ident);
    puts(":\n");
  }
  puts("\tpush ebp\n");
  puts("\tmov ebp, esp\n");
}

void epilogue() {
  puts("\tleave\n");
  puts("\tret\n");
}

void parseandemitblock(int nlocalexblock, int continuelabel, int breaklabel) {
  consumelbrace();
  int nlocal = nlocalexblock;
  for (; !matchtok(RBRACE);) {
    if (matchtok(BREAK)) {
      if (!breaklabel) {
        error("break can only appear inside of a loop");
      }
      consumesemi();
      emitjmp(breaklabel);
    } else if (matchtok(CONTINUE)) {
      if (!continuelabel) {
        error("continue can only appear inside of a loop");
      }
      consumesemi();
      emitjmp(continuelabel);
    } else if (matchtok(FOR)) {
      consumelparen();
      int n = nlocal;
      if (Peektok == SEMI) {
        n = nlocal;
      } else {
        n = parseandemitexprstmtorvardecl(nlocal, nlocal);
      }
      consumesemi();
      int beginlabel = newlabel();
      emitlabel(beginlabel);
      int newbreaklabel = newlabel();
      if (Peektok != SEMI) {
        int condexpr = derefint(parseorexpr(n));
        if (Exprtype[condexpr] != TYPEINT) {
          error("condition can only have integer type");
        }
        emitexpr(condexpr);
        emitcondjmp("z", newbreaklabel);
      }
      consumesemi();
      int increxpr = NOEXPR;
      if (Peektok != RPAREN) {
        increxpr = parseexprstmt(n);
      }
      consumerparen();
      int newcontinuelabel = newlabel();
      parseandemitblock(n, newcontinuelabel, newbreaklabel);
      emitlabel(newcontinuelabel);
      if (increxpr) {
        emitexpr(increxpr);
      }
      emitjmp(beginlabel);
      emitlabel(newbreaklabel);
    } else if (matchtok(IF)) {
      int endlabel = newlabel();
      for (;;) {
        consumelparen();
        int condexpr = derefint(parseorexpr(nlocal));
        if (Exprtype[condexpr] != TYPEINT) {
          error("condition can only have integer type");
        }
        emitexpr(condexpr);
        consumerparen();
        int elselabel = newlabel();
        emitcondjmp("z", elselabel);
        parseandemitblock(nlocal, continuelabel, breaklabel);
        if (!matchtok(ELSE)) {
          emitlabel(elselabel);
          break;
        }
        emitjmp(endlabel);
        emitlabel(elselabel);
        if (!matchtok(IF)) {
          parseandemitblock(nlocal, continuelabel, breaklabel);
          break;
        }
      }
      emitlabel(endlabel);
    } else if (matchtok(RETURN)) {
      int rettype = Rettype[Nfunc];
      if (Peektok == SEMI) {
        if (rettype != TYPEVOID) {
          error("must return a value");
        }
      } else if (rettype != TYPEINT) {
        error("must not return a value");
      } else {
        int retexpr = derefint(parseorexpr(nlocal));
        if (Exprtype[retexpr] != TYPEINT) {
          error("return value can only have type int");
        }
        emitexpr(retexpr);
      }
      consumesemi();
      epilogue();
    } else {
      nlocal = parseandemitexprstmtorvardecl(nlocalexblock, nlocal);
      consumesemi();
    }
  }
}

void parseandemitfuncdeclrest(int noret, int rettype, int ident) {
  if (noret) {
    if (rettype != TYPEVOID) {
      error("noreturn functions can only have return type void");
    }
    rettype = TYPENORETURN;
  } else if (rettype != TYPEINT && rettype != TYPEVOID) {
    error("functions can only have return type int or void");
  }
  int state = Globalstate[ident];
  int prevfunc;
  if (state) {
    int type = Exprtype[Globalexpr[ident]];
    if (type >= 0) {
      error("identifier declared previously as non-function");
    }
    prevfunc = -type;
  } else {
    prevfunc = 0;
    Globalstate[ident] = STATEDECLUNDEF;
  }
  nexttok();
  parseparamlist(prevfunc, rettype);
  consumerparen();
  if (!state) {
    Globalexpr[ident] = newunexpr(EXPRGLOBAL, -Nfunc, ident);
  }
  if (Peektok != LBRACE) {
    consumesemi();
    return;
  }
  if (state == STATEDEF) {
    error("function defined previously");
  }
  prologue(ident);
  int begin = Paramlistend[Nfunc - 1];
  int n = Paramlistend[Nfunc] - begin;
  for (int i = 0; i < n; i++) {
    Localident[i] = Paramident[begin + i];
    int offset = i + 2;
    int type = Paramtype[begin + i];
    if (type == TYPECHARARR) {
      Localexpr[i] = newunexpr(EXPRDEREF, TYPECHARARR,
                               newunexpr(EXPRLOCAL, TYPECHARARRREF, offset));
    } else if (type == TYPEINT) {
      Localexpr[i] = newunexpr(EXPRLOCAL, TYPEINTREF, offset);
    } else if (type == TYPEINTARR) {
      Localexpr[i] = newunexpr(EXPRDEREF, TYPEINTARR,
                               newunexpr(EXPRLOCAL, TYPEINTARRREF, offset));
    } else {
      internalerror("parseandemitfuncdeclrest");
    }
  }
  parseandemitblock(n, 0, 0);
  if (rettype == TYPEVOID) {
    epilogue();
  } else {
    puts("\tud2\n");
  }
  Globalstate[ident] = STATEDEF;
}

void parseandemitdecllist() {
  for (; Peektok;) {
    if (matchtok(ENUM)) {
      consumelbrace();
      parseenumlist();
      consumerbrace();
      consumesemi();
      continue;
    }
    int noret = matchtok(NORETURN);
    int type = consumetype();
    int ident = peekident();
    nexttok();
    if (Peektok == LPAREN) {
      parseandemitfuncdeclrest(noret, type, ident);
      continue;
    }
    if (noret) {
      error("noreturn can only appear on functions");
    }
    if (Globalstate[ident]) {
      error("identifier declared previously");
    }
    int arg;
    if (Peektok == LBRACKET) {
      if (type == TYPECHAR) {
        type = TYPECHARARR;
      } else if (type == TYPEINT) {
        type = TYPEINTARR;
      } else {
        error("array elements can only have integer type");
      }
      nexttok();
      arg = evalconstexpr(parseorexpr(0));
      if (arg <= 0) {
        error("arrays can only have positive length");
      }
      consumerbracket();
      if (Peektok == ASGN) {
        error("arrays cannot be initialized");
      }
    } else if (type == TYPEINT) {
      type = TYPEINTREF;
      if (matchtok(ASGN)) {
        int initexpr = derefint(parseorexpr(0));
        if (Exprtype[initexpr] != TYPEINT) {
          error("initializer can only have integer type");
        }
        arg = evalconstexpr(initexpr);
      } else {
        arg = 0;
      }
    } else {
      error("scalars can only have type int");
    }
    consumesemi();
    Globalexpr[ident] = newbinexpr(EXPRGLOBAL, type, ident, arg);
    Globalstate[ident] = STATEDEF;
  }
}

void section(char name[], int align) {
  putc('\n');
  puts("\tsection .");
  puts(name);
  puts(" align=");
  putdln(align);
}

void initprim() {
  int i = Paramlistend[Nfunc];

  Startident = keyword("start", IDENT);
  Nfunc++;
  Rettype[Nfunc] = TYPENORETURN;
  Paramlistend[Nfunc] = i;
  Globalexpr[Startident] = newunexpr(EXPRGLOBAL, -Nfunc, Startident);
  Globalstate[Startident] = STATEDECLUNDEF;

  int exitident = keyword("prim_exit", IDENT);
  Nfunc++;
  Rettype[Nfunc] = TYPENORETURN;
  Paramtype[i] = TYPEINT;
  i++;
  Paramlistend[Nfunc] = i;
  Globalexpr[exitident] = newunexpr(EXPRGLOBAL, -Nfunc, exitident);
  prologue(exitident);
  puts("\tsub esp, 8\n");
  puts("\tand esp, -16\n");
  puts("\tmov ebx, [ebp + 8]\n");
  puts("\tmov [esp + 4], ebx\n");
  puts("\tmov eax, 1\n");
  puts("\tint 128\n");
  Globalstate[exitident] = STATEDEF;

  int readident = keyword("primread", IDENT);
  Nfunc++;
  Rettype[Nfunc] = TYPEINT;
  Paramtype[i] = TYPEINT;
  i++;
  Paramtype[i] = TYPECHARARR;
  i++;
  Paramtype[i] = TYPEINT;
  i++;
  Paramlistend[Nfunc] = i;
  Globalexpr[readident] = newunexpr(EXPRGLOBAL, -Nfunc, readident);
  prologue(readident);
  puts("\tsub esp, 16\n");
  puts("\tand esp, -16\n");
  puts("\tmov edx, [ebp + 16]\n");
  puts("\tmov [esp + 12], edx\n");
  puts("\tmov ecx, [ebp + 12]\n");
  puts("\tmov [esp + 8], ecx\n");
  puts("\tmov ebx, [ebp + 8]\n");
  puts("\tmov [esp + 4], ebx\n");
  puts("\tmov eax, 3\n");
  puts("\tint 128\n");
  epilogue();
  Globalstate[readident] = STATEDEF;

  int writeident = keyword("primwrite", IDENT);
  Nfunc++;
  Rettype[Nfunc] = TYPEINT;
  Paramtype[i] = TYPEINT;
  i++;
  Paramtype[i] = TYPECHARARR;
  i++;
  Paramtype[i] = TYPEINT;
  i++;
  Paramtype[i] = TYPEINT;
  i++;
  Paramlistend[Nfunc] = i;
  Globalexpr[writeident] = newunexpr(EXPRGLOBAL, -Nfunc, writeident);
  prologue(writeident);
  puts("\tsub esp, 16\n");
  puts("\tand esp, -16\n");
  puts("\tmov edx, [ebp + 20]\n");
  puts("\tmov [esp + 12], edx\n");
  puts("\tmov ecx, [ebp + 16]\n");
  puts("\tadd ecx, [ebp + 12]\n");
  puts("\tmov [esp + 8], ecx\n");
  puts("\tmov ebx, [ebp + 8]\n");
  puts("\tmov [esp + 4], ebx\n");
  puts("\tmov eax, 4\n");
  puts("\tint 128\n");
  epilogue();
  Globalstate[writeident] = STATEDEF;
}

void assertnoundef() {
  for (int i = 1; i <= Nident; i++) {
    if (Globalstate[i] == STATEDECLUNDEF) {
      error("undefined function");
    }
  }
}

void emitstrlit() {
  for (int i = 1; i <= Nstrlit; i++) {
    puts("strlit");
    putd(i);
    puts(":\n");
    for (int j = Strlitend[i - 1]; j < Strlitend[i]; j++) {
      puts("\tdb ");
      putdln(Strlitbuf[j]);
    }
  }
}

void emitglobal(int type, char res[]) {
  for (int i = 1; i <= Nident; i++) {
    int expr = Globalexpr[i];
    if (Exprkind[expr] == EXPRGLOBAL && Exprtype[expr] == type) {
      puts("ident");
      putd(i);
      puts(":\n");
      putc('\t');
      puts(res);
      putc(' ');
      putdln(Exprarg2[expr]);
    }
  }
}

int main() {
  initlex();
  puts("\tglobal _start\n");
  puts("\tglobal start\n");
  section("text", 16);
  initprim();
  parseandemitdecllist();
  assertnoundef();
  section("rodata", 1);
  emitstrlit();
  section("data", 4);
  emitglobal(TYPEINTREF, "dd");
  section("bss", 4);
  emitglobal(TYPEINTARR, "resd");
  emitglobal(TYPECHARARR, "resb");
  return EXIT_SUCCESS;
}
