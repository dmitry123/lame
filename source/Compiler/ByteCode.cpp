#include "ByteCode.h"
#include "SegmentBuilder.h"

LAME_BEGIN2(Compiler)

ByteCodePtr ByteCode::New(Asm command) {

	if (!this->infoList.empty()) {
		this->Flush();
	}

	if (command == NOOP) {
		//return this;
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

	this->infoList.clear();

	return this;
}

Void ByteCode::Trace(SegmentBuilderPtr segmentBuilder) {

	SegmentPtr codeSegment = segmentBuilder->GetCodeSegment();
	SegmentPtr dataSegment = segmentBuilder->GetDataSegment();
	SegmentPtr textSegment = segmentBuilder->GetTextSegment();

	ObjectPtr object = NULL;
	Uint32 address = 0;
	Uint64 scopeHash;

	for (Uint32 i = 0; i < codeSegment->GetSize();) {

		AsmInfoPtr asmInfo = Assembler::GetAsmInfo(*codeSegment->GetBlockAt(i++));
		Uint32 argCount = asmInfo->arguments;

		if (strlen(asmInfo->name) <= 2) {
			printf("0x%.4x : %s\t\t\t", i + codeSegment->GetOffset(), asmInfo->name);
		}
		else if (strlen(asmInfo->name) >= 7) {
			printf("0x%.4x : %s\t", i + codeSegment->GetOffset(), asmInfo->name);
		}
		else {
			printf("0x%.4x : %s\t\t", i + codeSegment->GetOffset(), asmInfo->name);
		}

		while (argCount--) {

			address = *Uint32P(codeSegment->GetBlockAt(i));

			printf("0x%.4x ", address);

			if ((object = dataSegment->Fetch(address)) ||
				(object = textSegment->Fetch(address))
			) {
				scopeHash = object->GetPath().GetHash64();

				printf("// %.8x%.8x : %s", Uint32(scopeHash >> 32), Uint32(scopeHash),
					object->GetName().data());
			}

			i += 4;
		}

		printf("\n");
	}
}

LAME_END2