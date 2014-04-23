do {
	class A {
		private static final int value = 0x7b;
		public int getValue() {
			return this.value;
		}
	}

	int condition = 1;
	int state = 0;

	if (condition == 1) {
		A a = new A();
		state = a.getValue();
	} else if (condition == 2) {
		A  a = new A();
		state = a.getValue();
	}
}