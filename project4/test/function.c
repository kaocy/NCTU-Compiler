
int gcd (int n[10][2], float m);
int test(double x, float y, int z[5], bool b);
double test2(string s, int a);

double test2(string s, int a[2][3][5]){ // error
	int b[5];
	test(1, 1, 1.0, "abc"); // 2 error
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
	if (m==5) {
		return -1.0; // error
	}
	m = 0;
} // error

void test(){ // 2 error
	test4(1); // error
	int x = 1;
	return false; // error
}
void test(){ // 3 error
	test10(); // error
	int x = 1;
}
// error