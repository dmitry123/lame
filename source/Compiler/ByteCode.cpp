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

	this->segment->Write(&asmInfo->cmd, 1);

	if (asmInfo->args == this->infoList.size()) {
		this->Flush();
	}

	return this;
}

ByteCodePtr ByteCode::Write(Uint32 address) {

	this->infoList.push_back(address);

	if (asmInfo->args == this->infoList.size()) {
		this->Flush();
	}

	return this;
}

ByteCodePtr ByteCode::Flush(Void) {

	Uint32 address = 0;

	if (asmInfo->args != infoList.size()) {
        return this;
	}

	if (!infoList.empty()) {
		address = infoList[0];
	}

	for (Sint32 i = 0; i < asmInfo->args; i++) {
		this->segment->Write(&infoList[i], 4);
	}

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

		AsmInfoPtr asmInfo = Assembler::GetAsmInfo(*codeSegment->GetBlockAt(i));
		Uint32 argCount = asmInfo->args;
		Uint32 address = i + codeSegment->GetOffset();

		if ((object = codeSegment->Fetch(address)) && object->CheckType(Object::Type::Method)) {
			printf("  // %s %s%s(%s)\n", object->GetMethod()->GetReturnType()->GetName().data(),
				object->GetPath().data(), object->GetName().data(), object->GetMethod()->GetFormattedArguments().data());
		}

		if (strlen(asmInfo->name) <= 2) {
			printf("0x%.4x : %s\t\t\t", address, asmInfo->name);
		}
		else if (strlen(asmInfo->name) >= 7) {
			printf("0x%.4x : %s\t", address, asmInfo->name);
		}
		else {
			printf("0x%.4x : %s\t\t", address, asmInfo->name);
		}

		++i;

		while (argCount--) {

			address = *Uint32P(codeSegment->GetBlockAt(i));

			printf("0x%.4x ", address);

			if (asmInfo->cmd == IRLOAD ||
				asmInfo->cmd == LRLOAD ||
				asmInfo->cmd == FRLOAD ||
				asmInfo->cmd == DRLOAD ||
				asmInfo->cmd == RRLOAD ||
				asmInfo->cmd == BRLOAD ||
				asmInfo->cmd == CRLOAD ||
				asmInfo->cmd == SRLOAD ||
				asmInfo->cmd == IRSTORE ||
				asmInfo->cmd == LRSTORE ||
				asmInfo->cmd == FRSTORE ||
				asmInfo->cmd == DRSTORE ||
				asmInfo->cmd == RRSTORE ||
				asmInfo->cmd == BRSTORE ||
				asmInfo->cmd == CRSTORE ||
				asmInfo->cmd == SRSTORE ||
				asmInfo->cmd == IALOAD ||
				asmInfo->cmd == LALOAD ||
				asmInfo->cmd == FALOAD ||
				asmInfo->cmd == DALOAD ||
				asmInfo->cmd == RALOAD ||
				asmInfo->cmd == BALOAD ||
				asmInfo->cmd == CALOAD ||
				asmInfo->cmd == SALOAD ||
				asmInfo->cmd == IASTORE ||
				asmInfo->cmd == LASTORE ||
				asmInfo->cmd == FASTORE ||
				asmInfo->cmd == DASTORE ||
				asmInfo->cmd == RASTORE ||
				asmInfo->cmd == BASTORE ||
				asmInfo->cmd == CASTORE ||
				asmInfo->cmd == SASTORE ||
				asmInfo->cmd == CLEAR ||
				asmInfo->cmd == ICLD
			) {
				goto _Skip;
			}

			if ((object = dataSegment->Fetch(address)) ||
				(object = textSegment->Fetch(address)) ||
				(object = codeSegment->Fetch(address))
			) {
				if (asmInfo->cmd == RNEW) {
					goto _Skip;
				}

				scopeHash = object->GetPath().GetHash64();

				if (object->CheckType(Object::Type::Method)) {
					printf("   // %s %s%s(%s)", object->GetMethod()->GetReturnType()->GetName().data(),
						object->GetPath().data(), object->GetName().data(), object->GetMethod()->GetFormattedArguments().data());
				}
				else {
					printf("   // %s", object->GetName().data());
				}
			}

		_Skip:
			i += 4;
		}

		printf("\n");
	}
}

LAME_END2