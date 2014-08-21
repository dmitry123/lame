#include "ByteCodePrinter.h"

LAME_BEGIN2(Compiler)

ByteCodePrinterPtr ByteCodePrinter::byteCodePrinter_
	= new ByteCodePrinter();

ByteCodePrinterPtr ByteCodePrinter::New(Asm command) {

	if (!this->infoList.empty()) {
		this->Flush();
	}

	if (command == NOOP) {
		return this;
	}

	asmInfo = assembler.GetAsmInfo(
		command);

	if (asmInfo->arguments == this->infoList.size()) {
		this->Flush();
	}

	return this;
}

ByteCodePrinterPtr ByteCodePrinter::Write(Uint32 address, LineType lineType) {

	this->infoList.push_back({
		lineType, address
	});

	if (asmInfo->arguments == this->infoList.size()) {
		this->Flush();
	}

	return this;
}

ByteCodePrinterPtr ByteCodePrinter::Write(BufferRefC registerName) {

	this->infoList.push_back({
		LineType::Register, 0x00, registerName
	});

	if (asmInfo->arguments == this->infoList.size()) {
		this->Flush();
	}

	return this;
}

ByteCodePrinterPtr ByteCodePrinter::Flush(Void) {

	Uint32 address = 0;

	if (asmInfo->arguments != infoList.size()) {
        return this;
	}

	if (!infoList.empty()) {
		address = infoList[0].address;
	}

	if (strlen(asmInfo->name) >= 7) {
		printf("0x%.4x : %s\t", this->currentPosition, asmInfo->name);
	}
	else {
		printf("0x%.4x : %s\t\t", this->currentPosition, asmInfo->name);
	}
//	printf("0x%.4x : 0x%.2x ", this->currentPosition, asmInfo->command);
	if (asmInfo->arguments > 0) {
		printf("0x");
	}
	for (Sint32 i = 0; i < asmInfo->arguments; i++) {
		if (address <= 0xff) {
			printf("%.4x ", address);
		}
		else if (address <= 0xffff) {
			printf("%.4x ", address);
		}
		else {
			printf("%.4x ", address);
		}
	}
	puts("");

	this->currentPosition += asmInfo->arguments * 4 + 1;

	this->infoList.clear();

	return this;
}

LAME_END2