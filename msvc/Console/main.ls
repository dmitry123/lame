class A {
	public void foo(int mulA, int mulB, int mulC) {

		char a = 1;
		char b = 2;
		char c = 3;

		short z =
			a * mulA +
			b * mulB +
			c * mulC;

		return z;
	}
}

A a = new A();

a.foo(100, 10, 1);