int foo(int a[3][3], int b[5][3], int i, float j) {
	a[0][0] = i; // legal
	i = a[1][1]; // legal
	a[0.5][j] = 1; // illegal
	a[0][1] = b[1][2]; // legal
	int k = a[3][i]; // legal : note that bounds are not checked
	i = 3+a[0]; // illegal : array arithmetic
	a = b; // illegal : array assignment
	a[1] = b[2]; // illegal : array assignment

	double x = i+j;
	i = i%j; // illegal
	i = i-j; // illegal
	x = (i*j) + (i%i) + x;
	j = (i*j) + (i%i) + x; // illegal

	bool b1, b2;
	if (1+1 < 1.5*1.5) {}
	if (b1 == b2) {}
	while (b1 >= b2) {} // illegal
	if (i>j && b[5][3]!=x) {}
	if (x>j || 1) {} // illegal
	if (!(b[5][3]!=x)) {}
	if (!1) {} // illegal

	return a[0][0]; // legal : ’a[0][0]’ is a scalar type, but ’a’ is an array type.
}

int f1(int a[5]);
int f2(int a);
int f3(float a[4]);
void f4(int x, int z[5][5][5], int t[4]) {
	x = f1(z[0][1]); // legal
	x = f1(t); // illegal : size mismatch
	x = f1(z[0][0][1]); // illegal
	x = f2(z[0][0][1]); // legal
	x = f3(t); // legal : type coercion (see below)

	string a, b;
	a = "compiler"; // legal
	b = a;          // legal
	b = b + a;      // illegal
}
