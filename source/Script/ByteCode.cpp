#include "ByteCode.h"
#include "SegmentBuilder.h"

LAME_BEGIN2(Script)

ByteCodePtr ByteCode::New(Asm command) {

	if (!this->infoList.empty()) {
		this->Flush();
	}

#if 0 /* Deprecate noop instruction */
	if (command == NOOP) {
		return this;
	}
#endif

	this->asmInfo = Assembler::GetInstance()
		->GetInfo(command);

	this->segment->Write(&asmInfo->code, 1);

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

		Assembler::AsmInfoPtrC asmInfo = Assembler::GetInstance()
			->GetInfo(*codeSegment->GetBlockAt(i));

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

			if (asmInfo->code == IRLOAD ||
				asmInfo->code == LRLOAD ||
				asmInfo->code == FRLOAD ||
				asmInfo->code == DRLOAD ||
				asmInfo->code == RRLOAD ||
				asmInfo->code == BRLOAD ||
				asmInfo->code == CRLOAD ||
				asmInfo->code == SRLOAD ||
				asmInfo->code == IRSTORE ||
				asmInfo->code == LRSTORE ||
				asmInfo->code == FRSTORE ||
				asmInfo->code == DRSTORE ||
				asmInfo->code == RRSTORE ||
				asmInfo->code == BRSTORE ||
				asmInfo->code == CRSTORE ||
				asmInfo->code == SRSTORE ||
				asmInfo->code == IALOAD ||
				asmInfo->code == LALOAD ||
				asmInfo->code == FALOAD ||
				asmInfo->code == DALOAD ||
				asmInfo->code == RALOAD ||
				asmInfo->code == BALOAD ||
				asmInfo->code == CALOAD ||
				asmInfo->code == SALOAD ||
				asmInfo->code == IASTORE ||
				asmInfo->code == LASTORE ||
				asmInfo->code == FASTORE ||
				asmInfo->code == DASTORE ||
				asmInfo->code == RASTORE ||
				asmInfo->code == BASTORE ||
				asmInfo->code == CASTORE ||
				asmInfo->code == SASTORE ||
				asmInfo->code == ICLD ||
				asmInfo->code == RNEW
			) {
				goto _Skip;
			}

			if ((object = dataSegment->Fetch(address)) ||
				(object = textSegment->Fetch(address)) ||
				(object = codeSegment->Fetch(address))
			) {
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