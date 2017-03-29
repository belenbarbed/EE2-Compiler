int Fibonacci(int n) {
	
	int res = 0;
	
	if ( n == 0 ) {
	
	} else {
		if ( n == 1 ) {
			res = 1;
		} else {
			res = ( Fibonacci(n - 1) + Fibonacci(n - 2) );
		}
	}
	
	return res;
}
