class A {}
class B {}
class C {}

thread ("Main") {
	Int a = 0xff;
	Int c = 0x00;
	Int b = 0x00;

	b = a;
	while (a--) {
		++c;
	}

	a = b;
	do {
		++c;
	} while (a--);
}