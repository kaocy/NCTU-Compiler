#pragma symbol on

int x;
void main() {
	int a;
	bool b;
	if (true) {
		double c, d;
		while (true) {
			float e, f;
		}
	}
	int g, h;
}

void test(int x, int y) {
	int a;
	bool b;
	if (true) {
		double a, d;
		while (true) {
			float e, f;
		}
	}
	int g, h;
	return 1;
}

void test2() {
	if (true) {
		double c, d;
		while (true) {
			float c, d;
		}
	}
	int g, h;
}