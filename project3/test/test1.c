#pragma symbol on

bool func(int a, float b);
const int d = 1;

void main() {
	int x;
	int y[10], x[2][9999];
	if (y[0] == 0) {
		float x; //outer ’x’ has been hidden in this scope
		double z, z = 0.0;
		z = 0.1;
	}
}

bool func (int a, float b) {
	const int d = 1, dd = 2;
	const string test1 = "hi";
	const bool test2 = true;
	string c = "hello world";
	return (b <= 1.0);
}