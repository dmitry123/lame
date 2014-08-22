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

	//printf(" + %s  \t", asmInfo->name);

	for (Sint32 i = 0; i < asmInfo->arguments; i++) {
		//printf("0x%.4x ", infoList[i]);
		this->segment->Write(&infoList[i], 4);
	}

	//printf("\n");

	this->position += asmInfo->arguments * 4 + 1;
	this->infoList.clear();

	return this;
}

Void ByteCode::Trace(SegmentPtr segment) {

	for (Uint32 i = 0; i < segment->GetSize();) {

		AsmInfoPtr asmInfo = Assembler::GetAsmInfo(*segment->GetBlockAt(i++));
		Uint32 argCount = asmInfo->arguments;

		if (strlen(asmInfo->name) >= 7) {
			printf("0x%.4x : %s\t", i + segment->GetOffset(), asmInfo->name);
		}
		else {
			printf("0x%.4x : %s\t\t", i + segment->GetOffset(), asmInfo->name);
		}

		while (argCount--) {
			printf("0x%.4x ", *Uint32P(segment->GetBlockAt(i)) + 8);
			i += 4;
		}
		printf("\n");
	}
}

LAME_END2