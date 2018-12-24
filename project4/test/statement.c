
int main() {
	int a[3];
	string s[10][2][3];
	read a; // error
	read s[1][1][1];
	print a[2];
	print s[9]; // error

	double x;
	float y;
	int z;
	string ss;
	x = y;
	x = z;
	y = z;
	z = x; // error
	z = y; // error
	ss = z; // error

	while(true) {
		continue;
		if (1) { // error
			continue;
		}
		bool flag;
		for (i=0; flag=true; i=i+1) { // 6 error
			break;
		}

	}
	break; // error
	continue; // error
	return 0;
}