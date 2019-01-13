
int x;
// int x1 = 2, x2 = 3;
// float y1 = 4, y2 = 2 + 3.0;
// double z = 0.5, z1;

void main() {
	x = 2;
	int a = 2;
	float b = 3.3;
	double c = 4.4;
	print a + b / x;
	print "\n";

	// z1 = 0.1;
	// print (x1 + x2 + a) / z1;
	// print "\n";
	// print y1 + y2 + z - b - c;
	// print "\n";

	print c - a;
	print "\n";

	bool b1, b2;
	b1 = true;
	b2 = false;
	print !(b2 || b1 && b2);

	//print (b1 == b2);
	print "\n";

	const int con = 1;
	double r1, r2 = con;
	read r1;
	print r1 - r2;
	print "\n";
}

/*

Input:
5.123

Output:
3.65
2.4
true
4.123

*/