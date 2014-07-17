#include "ByteCodePrinter.h"

LAME_BEGIN2(Compiler)

ByteCodePrinterPtr ByteCodePrinter::byteCodePrinter_
	= new ByteCodePrinter();

ByteCodePrinterPtr ByteCodePrinter::New(Asm command) {

	if (!this->infoList.empty()) {
		this->Flush();
	}

	asmInfo = assembler.GetAsmInfo(
		command);

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

	if (!this->infoList.size()) {
		return this;
	}

	if (asmInfo->arguments != infoList.size()) {
		__asm int 3
	}

	printf("0x%.4x : %s ", this->currentPosition, asmInfo->name);

	for (Uint32 i = 0; i < asmInfo->arguments; i++) {

		if (infoList[i].lineType == LineType::Address) {
			printf("<0x%.4x>", infoList[i].address);
		} else if (infoList[i].lineType == LineType::Jump) {
			printf("0x%.4x", infoList[i].address);
		} else if (infoList[i].lineType == LineType::Const) {
			printf("#%d", infoList[i].address);
		} else {
			printf("%s", infoList[i].registerName.data());
		}

		if (i < asmInfo->arguments - 1) {
			printf(" ");
		}
	}
	puts("");

	this->currentPosition += this->infoList.size() * 4 + 1;
	this->infoList.clear();

	return this;
}

LAME_END2