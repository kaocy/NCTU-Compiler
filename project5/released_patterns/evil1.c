
const float f1 = 3.0;
float add(float a, float b);

int main() {
	float f2;
	read f2;

	int a = 1 + 1;
	if (true) {
		float x;
		for (x = 0; x < add(1.0, 7.0); x = x + a) {
			print add(f1 * 2 / 3, add(f2, f2));
			print " ";
		}
		print "\n";
	}

	return 0;
}

float add(float a, float b) {
	return a + b;
}

/*

Input:
1.2

Output:
4.4 4.4 4.4 4.4

*/