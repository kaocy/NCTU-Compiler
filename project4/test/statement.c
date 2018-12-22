
int main() {
	int a[3];
	read a; // error
	print a[2];

	double x;
	float y;
	int z;
	string s;
	x = y;
	x = z;
	y = z;
	z = x; // error
	z = y; // error
	s = z; // error

	while(true) {
		continue;
		if (1) { // error
			continue;
		}
		for (i=0; false; i=i+1) { // error
			break;
		}

	}
	break;
	continue;
	return 0;
}