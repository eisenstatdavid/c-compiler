enum {
  A = 314159,
  B = -2718,
  C = "\145"[0],
  D = -A,
  E1 = !A,
  E2 = !E1,
  F = +A,
  G = A + B,
  H1 = A && A,
  H2 = A && E1,
  H3 = E1 && A,
  H4 = E1 && E1,
  I = A / B,
  J1 = A == A,
  J2 = A == B,
  J3 = B == A,
  J4 = B == B,
  K1 = A >= A,
  K2 = A >= B,
  K3 = B >= A,
  K4 = B >= B,
  L1 = A > A,
  L2 = A > B,
  L3 = B > A,
  L4 = B > B,
  M1 = A <= A,
  M2 = A <= B,
  M3 = B <= A,
  M4 = B <= B,
  N1 = A < A,
  N2 = A < B,
  N3 = B < A,
  N4 = B < B,
  O = A % B,
  P = A * B,
  Q1 = A != A,
  Q2 = A != B,
  Q3 = B != A,
  Q4 = B != B,
  R1 = A || A,
  R2 = A || E1,
  R3 = E1 || A,
  R4 = E1 || E1,
  S = A - B,
};

int main() {
  putdln(A);
  putdln(B);
  putdln(C);
  putdln(D);
  putdln(E1);
  putdln(E2);
  putdln(F);
  putdln(G);
  putdln(H1);
  putdln(H2);
  putdln(H3);
  putdln(H4);
  putdln(I);
  putdln(J1);
  putdln(J2);
  putdln(J3);
  putdln(J4);
  putdln(K1);
  putdln(K2);
  putdln(K3);
  putdln(K4);
  putdln(L1);
  putdln(L2);
  putdln(L3);
  putdln(L4);
  putdln(M1);
  putdln(M2);
  putdln(M3);
  putdln(M4);
  putdln(N1);
  putdln(N2);
  putdln(N3);
  putdln(N4);
  putdln(O);
  putdln(P);
  putdln(Q1);
  putdln(Q2);
  putdln(Q3);
  putdln(Q4);
  putdln(R1);
  putdln(R2);
  putdln(R3);
  putdln(R4);
  putdln(S);
  return EXIT_SUCCESS;
}
