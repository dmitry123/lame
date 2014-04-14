class Main {
	String stringValue;
	Int intValue;
	Float floatValue;
	Bool boolValue;
}

thread ("Main") {
	
	Main main = new Main();

	main.stringValue = 'hello, world';
	main.intValue = 0x7b;
	main.floatValue = 7.5f;
	main.boolValue = 1;
}