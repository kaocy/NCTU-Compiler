
double foo(){
	int a[3] = {1,2,3}; // legal
	int b[4] = {1,2,3,4,5}; // illegal
	int c[2][3] = {1,2,3,4,5,6,7}; // illegal
	int d[0] = {1}; // illegal
	int e = "This is a string"; // illegal

	int w = 1.0; // error
	float x = 1;
	double y = w;
	string s = true; // error
	return 0;
}

void foo1() {
	int x;
	float y;
	double z;
	float a[5] = {1, 2.0, 1e9, "abc", x*y}, b[1][1]={foo()}; // 3 error
	double c[5] = {1, 2.0, 1e9, "abc", x*y}, d[1][1]={foo()}; // 1 error
}

void foo2() {
	const int a=-1, b=2.0; // error
	const float c=2, c2=1e9, c3=0.0; // error
	const bool d="ss"; // error
	const string s="hello";
	float x;

	a = 1;
	x = 0;
	c = 2;
	a = 1.0;
}