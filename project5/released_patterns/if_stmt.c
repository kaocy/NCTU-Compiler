void main(){ 
	float a, b;
	
	read a;
	read b;
	
	if( a > b ) {
		if (1 == 1) {
			print "1 is equal to 1\n";
		}
		if (2 != 3) {
			print "2 is not equal to 3\n";
		}
		print a;
		print " is greater than ";
		print b;
		print "\n";
	}
	
	else {
		if( a == b ) {
			print a;
			print " is equal to ";
			print b;
			print "\n";
		}

		print a;
		print " is less than or equal to ";
		print b;
		print "\n";
	}

	if (true != (3 < 2)) {
		print "test\n";
	}
}


/*

Input:
1.732
1.414

Output:
1 is equal to 1
2 is not equal to 3
1.732 is greater than 1.414

Input:
1.414
1.414

Output:
1.414 is equal to 1.414
1.414 is less than or equal to 1.414

*/

