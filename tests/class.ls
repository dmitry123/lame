class A {
	Int intValue = Int(0x7b);
	Float floatValue = Float(0x7b);
	Bool boolValue = Bool(0x7b);
	String stringValue = String(0x7b);
}

thread("Main") {
	
	A classNull;
	A classNew = new A();
}