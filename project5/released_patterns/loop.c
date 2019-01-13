
void main() {
	int y = 3;
	do {
		int x = 2;
		while (x > 0) {
			int i;
			for (i = 0; i < 5; i = i + 1) {
				int j;
				for (j = 10; j >= 0; j = j - 1) {
					print j - i;
					print " ";
				}
				print "\n";
				for (j = 10; j >= 0; j = j - 1) {
					print j + i;
					print " ";
				}
				print "\n";
			}
			print "\n";
			x = x - 1;
		}
		y = y - 1;
	} while(y > 0);
}