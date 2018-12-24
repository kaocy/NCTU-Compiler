
double foo(){
	double a[3] = {1,2,3};
	int b[4] = {1,2,3,4,5}; // error
	int c[2][3] = {1,2,3,4,5,6,7}; // error
	bool d[0] = {1}; // 3 error
	int e = "This is a string"; // error

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

	for (a=1; a<0; a=x) { // error
		c = 2; // error
		x = 0;
		a = x; // 2 error
	}
}