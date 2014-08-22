#include "ByteCode.h"

LAME_BEGIN2(Compiler)

ByteCodePtr ByteCode::New(Asm command) {

	if (!this->infoList.empty()) {
		this->Flush();
	}

	if (command == NOOP) {
		return this;
	}

	asmInfo = assembler.GetAsmInfo(
		command);

	this->segment->Write(&asmInfo->command, 1);

	if (asmInfo->arguments == this->infoList.size()) {
		this->Flush();
	}

	return this;
}

ByteCodePtr ByteCode::Write(Uint32 address) {

	this->infoList.push_back(address);

	if (asmInfo->arguments == this->infoList.size()) {
		this->Flush();
	}

	return this;
}

ByteCodePtr ByteCode::Flush(Void) {

	Uint32 address = 0;

	if (asmInfo->arguments != infoList.size()) {
        return this;
	}

	if (!infoList.empty()) {
		address = infoList[0];
	}

	if (strlen(asmInfo->name) >= 7) {
		printf("0x%.4x : %s\t", this->position, asmInfo->name);
	}
	else {
		printf("0x%.4x : %s\t\t", this->position, asmInfo->name);
	}

	if (asmInfo->arguments > 0) {
		printf("0x");
	}

	for (Sint32 i = 0; i < asmInfo->arguments; i++) {

		address = infoList[i];

		this->segment->Write(&address, 4);

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

	this->position += asmInfo->arguments * 4 + 1;
	this->infoList.clear();

	return this;
}

LAME_END2