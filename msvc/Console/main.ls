class A {
	Int intValue = 0x7b;
	Void functionValue(Int value) {
		Int temp = 0x00;
	}
}

thread("Main") {
	A a = new A();
	a.functionValue(0xff);
}