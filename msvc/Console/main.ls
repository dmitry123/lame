class Main {
	String stringValue;
	Int intValue;
	Float floatValue;
	Bool boolValue;
}

thread ("Main") {
	
	Var left = new (Main);

	left.stringValue = 'hello, world';
	left.intValue = 0x7b;
	left.floatValue = 7.5f;
	left.boolValue = 1;
}