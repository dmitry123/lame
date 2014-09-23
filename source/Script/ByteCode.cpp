#include "ByteCode.h"
#include "SegmentBuilder.h"

LAME_BEGIN2(Script)

ByteCodePtr ByteCode::New(Instruction command) {

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

ByteCodePtr ByteCode::Write(Uint8 address) {

	this->infoList.push_back(address);

	if (asmInfo->args == this->infoList.size()) {
		this->Flush();
	}

	return this;
}

ByteCodePtr ByteCode::Flush(Void) {

	Uint32 address = 0;

	LAME_ASSERT(asmInfo->args == infoList.size());

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
			->GetInfo(Instruction(*codeSegment->GetBlockAt(i)));

		Uint32 argCount = asmInfo->args;
		Uint32 address = i + codeSegment->GetOffset();

		if ((object = codeSegment->Fetch(address)) && object->CheckType(Object::Type::Method)) {
			printf("  // %s %s%s(%s)\n", object->GetMethod()->GetReturnType()->GetName().data(),
				object->GetPath().data(), object->GetName().data(), object->GetMethod()->FormatArguments().data());
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

			if (asmInfo->code == Instruction::IRLOAD ||
				asmInfo->code == Instruction::LRLOAD ||
				asmInfo->code == Instruction::FRLOAD ||
				asmInfo->code == Instruction::DRLOAD ||
				asmInfo->code == Instruction::RRLOAD ||
				asmInfo->code == Instruction::BRLOAD ||
				asmInfo->code == Instruction::SRLOAD ||
				asmInfo->code == Instruction::IRSTORE ||
				asmInfo->code == Instruction::LRSTORE ||
				asmInfo->code == Instruction::FRSTORE ||
				asmInfo->code == Instruction::DRSTORE ||
				asmInfo->code == Instruction::RRSTORE ||
				asmInfo->code == Instruction::BRSTORE ||
				asmInfo->code == Instruction::SRSTORE ||
				asmInfo->code == Instruction::IALOAD ||
				asmInfo->code == Instruction::LALOAD ||
				asmInfo->code == Instruction::FALOAD ||
				asmInfo->code == Instruction::DALOAD ||
				asmInfo->code == Instruction::RALOAD ||
				asmInfo->code == Instruction::BALOAD ||
				asmInfo->code == Instruction::SALOAD ||
				asmInfo->code == Instruction::IASTORE ||
				asmInfo->code == Instruction::LASTORE ||
				asmInfo->code == Instruction::FASTORE ||
				asmInfo->code == Instruction::DASTORE ||
				asmInfo->code == Instruction::RASTORE ||
				asmInfo->code == Instruction::BASTORE ||
				asmInfo->code == Instruction::SASTORE ||
				asmInfo->code == Instruction::RNEW
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
						object->GetPath().data(), object->GetName().data(), object->GetMethod()->FormatArguments().data());
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