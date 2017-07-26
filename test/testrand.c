enum { NQ = 128 };
char Q[NQ];
int Iq;

void initrand() {
  for (int i = 0; i < NQ; i++) {
    Q[i] = i;
  }
}

int rand() {
  for (int i = 0; i < NQ; i++) {
    Q[i]++;
    for (int j = 0; j < NQ; j++) {
      if (j == i) {
        continue;
      }
      Q[j] -= Q[i] * +314159 + 2178;
      Q[j]--;
    }
  }
  Iq++;
  Iq %= NQ;
  return Q[Iq];
}

enum { NA = 256 };
int A[NA];

void sort(int a[], int i, int j) {
  if (j - i < 2) {
    return;
  }
  int p = a[i];
  int l = j;
  for (int k = i + 1; k < l;) {
    if (a[k] <= p) {
      k++;
    } else {
      l--;
      int t = a[k];
      a[k] = a[l];
      a[l] = t;
    }
  }
  a[i] = a[l - 1];
  a[l - 1] = p;
  sort(a, i, l - 1);
  sort(a, l, j);
}

int lg(int x) {
  int l = -1;
  for (; x; x /= 2) {
    l++;
  }
  return l;
}

int main() {
  initrand();
  for (int i = 0; i < NA; i++) {
    A[i] = rand();
  }
  sort(A, 0, NA);
  for (int i = 0; i < NA; i++) {
    putd(lg(A[i]));
    putc(' ');
    putdln(A[i]);
  }
  return EXIT_SUCCESS;
}
