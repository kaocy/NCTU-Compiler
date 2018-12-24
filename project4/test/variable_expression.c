
int foo(int a[3][3], int b[5][3], int i, float j) {
	a[0][0] = i;
	i = a[1][1];
	a[0.5][j] = 1; // 2 error
	a[0][1] = b[1][2];
	int k = a[3][i];
	i = 3+a[0]; // 2 error
	a = b; // error
	a[1] = b[2]; // error

	double x = i+j;
	i = i%j; // 2 error
	i = i-j; // error
	x = (i*j) + (i%i) + x;
	j = (i*j) + (i%i) + x; // error

	bool b1, b2;
	if (1+1 < 1.5*1.5) {}
	if (b1 == b2) {}
	while (b1 >= b2) {} // error
	if (i>j && b[5][3]!=x) {}
	if (x>j || 1) {} // 2 error
	if (!(b[5][3]!=x)) {}
	if (!1) {} // 2 error

	return a[0][0]; // legal : ’a[0][0]’ is a scalar type, but ’a’ is an array type.
}

int f1(int a[5]);
int f2(int a);
bool f3(float a[4]);
void f4(int x, int z[5][5][5], int t[4]) {
	x = f1(z[0][1]);
	x = f1(t); // error : size mismatch
	x = f1(z[0][0][1]); // error
	x = f2(z[0][0][1]);
	x = f3(t);
	while (f3(t) && (t[3] < x) || !(x != z[0][0][0])) {}

	string a, b;
	a = "compiler";
	b = a;
	b = b + a;      // 2 error
}
