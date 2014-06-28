#include "Math.h"

LAME_BEGIN2(Core)

ElasticBox::ElasticBox() {

	this->children4 = 0;
	this->item_h = 0;
	this->item_w = 0;
	this->x = 0;
	this->y = 0;
	this->w = 100;
	this->h = 100;
}

ElasticBox::~ElasticBox() {

	if (this->children4) {
		for (Uint32 i = 0; i < 4; i++) {
			if (this->children4[i]) {
				delete this->children4[i];
			}
		}
		delete[] this->children4;
	}
}

Void ElasticBox::AdjustChildren() {

	if (this->children4[0]) {
		this->children4[0]->Resize(
			this->x + this->item_w, this->y, this->w - this->item_w, this->h - this->y);
	}
	if (this->children4[1]) {
		this->children4[1]->Resize(
			this->x, this->y + this->item_h, this->item_w, this->h - this->item_h);
	}
	if (this->children4[2]) {
		this->children4[2]->Resize(
			this->x, this->y + this->item_h, this->w - this->x, this->h - this->item_h);
	}
	if (this->children4[3]) {
		this->children4[3]->Resize(
			this->x + this->item_w, this->y, this->w - this->item_w, this->item_h);
	}
}

Void ElasticBox::Resize(Uint32 x, Uint32 y, Uint32 width, Uint32 height) {

	this->x = x;
	this->y = y;
	this->w = width;
	this->h = height;

	if (this->children4) {
		this->AdjustChildren();
	}
}

Uint32 ElasticBox::FindSpace(Uint32P x, Uint32P y, Uint32 item_w, Uint32 item_h) {

	Uint32 space_found = kNeedNothing;
	Uint32 i;

	if (item_w > w && item_h > h) {
		return kNeedMore;
	}
	if (item_w > w)	{
		return kNeedWidth;
	}
	if (item_h > h)	{
		return kNeedHeight;
	}

	if (!children4) {

		*x = this->x;
		*y = this->y;

		this->item_w = item_w;
		this->item_h = item_h;

		space_found = kNeedNothing;

		children4 = new ElasticBox*[4];

		for (i = 0; i < 4; i++) {
			children4[i] = new ElasticBox(w, h);
		}

		AdjustChildren();
	}
	else
	{
		for (i = 0; i < 4; i++) {
			if (this->children4[i] && (
				space_found = this->children4[i]->FindSpace(x, y, item_w, item_h)
			) == kNeedNothing) {
				break;
			}
		}

		if (space_found == kNeedNothing) {
			switch (i) {
			case 0:
			case 1:

				delete this->children4[2];
				delete this->children4[3];

				this->children4[2] = 0;
				this->children4[3] = 0;

				break;
			case 2:
			case 3:
				
				delete this->children4[0];
				delete this->children4[1];
				
				this->children4[0] = 0;
				this->children4[1] = 0;

				break;
			}
		}
	}

	return space_found;
}

Void ElasticBox::InsertItem(Uint32P x, Uint32P y, Uint32 width, Uint32 height) {

	Uint32 space_found;

	while ((space_found = FindSpace(x, y, width, height)) != kNeedNothing) {
		switch (space_found) {
		case kNeedWidth: {
			Resize(0, 0, this->w + 64, this->h);
		} break;
		case kNeedHeight: {
			Resize(0, 0, this->w, this->h + 64);
		} break;
		default: {
			Resize(0, 0, this->w + 64, this->h + 64);
		} break;
		}
	}
}

LAME_END2
