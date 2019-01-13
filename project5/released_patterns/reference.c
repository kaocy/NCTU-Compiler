int a;
float b;
bool c;

const int aa = 10;
const bool bb = true;
const string ss = "string";
const float rr = 1.23;

void main() {
	const int test = 4;
	int d;
	float e;
	bool f;

	a = 1;
	b = 1.0;
	c = true;
	d = 2;
	e = 2.0;
	f = false;

	int a1 = a;
	float b1 = b;
	bool c1 = c;
	int d1 = d;
	float e1 = e;
	bool f1 = f;

	int a2 = aa;
	bool b2 = bb;
	print ss;
	print "\n";
	float r2 = rr;

	print 10;
	print "\n";
	print false;
	print "\n";
	print "hello";
	print "\n";
	print 1.23;
	print "\n";
}

/*

Output:
string
10
false
hello
1.23

*/

