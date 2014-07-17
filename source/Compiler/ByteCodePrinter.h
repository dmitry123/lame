#ifndef __LAME_COMPILER_BYTE_CODE_PRINTER__
#define __LAME_COMPILER_BYTE_CODE_PRINTER__

#include "Assembler.h"

LAME_BEGIN2(Compiler)

class LAME_API ByteCodePrinter {
	typedef Core::Buffer Buffer;
	typedef Core::BufferRefC BufferRefC;
public:
	static ByteCodePrinterPtr GetInstance() {
		return byteCodePrinter_;
	}
public:
	enum class LineType {
		Address,
		Register,
		Jump,
		Const
	};
public:
	ByteCodePrinterPtr New(Asm command);
	ByteCodePrinterPtr Write(Uint32 address, LineType lineType = LineType::Address);
	ByteCodePrinterPtr Write(BufferRefC registerName);
	ByteCodePrinterPtr Flush(Void);
public:
	inline Uint32 GetPosition() {
		return this->currentPosition;
	}
	inline Void SetPosition(Uint32 position) {
		this->currentPosition = position;
	}
	inline Uint32 IsSource() {
		return this->infoList.size() == 1;
	}
private:
	typedef struct {
		LineType lineType;
		Uint32 address;
		Buffer registerName;
	} LineInfo;
private:
	Core::List<Buffer> lineList;
	Core::Vector<LineInfo> infoList;
	Assembler assembler;
	AsmInfoPtr asmInfo;
	Uint32 currentPosition;
private:
	ByteCodePrinter() :
		currentPosition(0)
	{
	}
private:
	static ByteCodePrinterPtr byteCodePrinter_;
};

LAME_END2

#endif // ~__LAME_COMPILER_BYTE_CODE_PRINTER__