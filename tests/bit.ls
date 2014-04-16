thread("Main") {

	Int value = 0x7b << 1;
	Int iterator = 17;
	String result;

	while (iterator--) {
		result += (value >> (iterator - 1)) & 1;
	}

	if ((value >> 1) & 1) {
		if (value & 0x06) {
			value = 0xff;
		} else {
			value = 0x00;
		}
	}
}