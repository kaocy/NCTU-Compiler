
int gcd (int n[10][2], float m);
int test(double x, float y, int z[5], bool b);
double test2(string s, int a);

double test2(string s, int a[2][3][5]){ // error
	int b[5];
	test(1, 1, 1.0, "abc"); // error
	test(1.0, 1, a[1][3], true);
	test(1.0, 1, b); // error
	test3(); // error
	int ans;
	return ans;
	s = "1";
} // error

double test2(string s, int a); // error
string test3();

void test4() {
	int yy=0;
}

int main() {
	return 0;
}

int gcd(int n[10][2], float m) {
	return -1.0; // error
	m = 0;
} // error

void test(){ // error
	test4(1); // error
	int x = 1;
}
void test(){ // error
	int x = 1;
}
