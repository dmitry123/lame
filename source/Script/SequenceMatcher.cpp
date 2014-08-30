#include "SequenceMatcher.h"
#include "Exception.h"

LAME_BEGIN2(Script)

Bool SequenceMatcher::Declare(Uint32 id, Vector<LexID> lexList) {

	if (this->lexMap_.count(id) || lexList.empty()) {
		return FALSE;
	}

	this->lexMap_[id] = lexList;

	return TRUE;
}

Bool SequenceMatcher::Match(Uint32 id, Iterator i, Iterator end) {

	if (!this->lexMap_.count(id)) {
		return FALSE;
	}

	Vector<LexID> lexList = this->lexMap_[id];
	Uint32 okCount = 0;

	if (end - i < (Sint32)lexList.size()) {
		return FALSE;
	}

	for (LexID lexID : lexList) {
		if (lexID != (*i++)->lex->id) {
			if (okCount >= 3 && FALSE) {
				PostSyntaxError((*i)->line, "Not finished expression at (%s) position", (*i)->word.data());
			}
			return FALSE;
		}
		else {
			++okCount;
		}
		if (end == i) {
			PostSyntaxError((*(i - 1))->line, "Not finished expression at (%s) position", (*(i - 1))->word.data());
		}
	}

	return TRUE;
}

LAME_END2