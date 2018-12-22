void f1(float a);
void f2(double a[10]);
int f3();
double f3(int a, float b, double c, int d[10]) {
	b = a; // a is converted from int to float
	c = a; // a is converted from int to double
	c = b; // b is converted from float to double
	c = a + b; // a is converted to float before addition,
	// which produces a float value that is converted to double.
	f1(a); // legal, a is converted from int to float
	c = f3(); // legal, the return type int is converted to double
	f2(d); // legal, d is converted from an array of 10 int’s to an array of 10 double’s
	a = c; // illegal, a double value cannot be converted to an integer
	while (b == c) {} // legal, b is converted to double before comparison
	return a; // legal, the return value is implicitly converted to double
}

void main() {

}