#include "Resource.h"

LAME_BEGIN2(ResourceManager)

Resource::~Resource() {

	if (this->resourceLoader_.audioLoader_) {
		if (this->resourceLoader_.audioLoader_->GetInfo()->fileType.IsAudio()) {
			delete this->resourceLoader_.audioLoader_;
		}
		else if (this->resourceLoader_.textureLoader_->GetInfo()->fileType.IsTexture()) {
			delete this->resourceLoader_.textureLoader_;
		}
	}
}

Bool Resource::Load(StringC fileName) {

	if (!fileName) {
		PostErrorMessage("Resource's filename mustn't be NULL", 1);
	}

	if (!File::IsFile(fileName)) {
		PostErrorMessage("Unable to open resource (%s)", fileName);
	}

	ResourceType resourceType(fileName);

	if (resourceType.IsAudio()) {

		switch (resourceType) {
		case kResourceTypeWav: this->resourceLoader_.audioLoader_ = new WavLoader; break;
		default: break;
		}

		if (!this->resourceLoader_.audioLoader_->Check(File::GetFileExtension(fileName))) {
			PostErrorMessage("Invalid file's extension (%s)", File::GetFileExtension(fileName));
		}

		this->resourceLoader_.audioLoader_->Load(fileName);

	}
	else if (resourceType.IsTexture()) {

		switch (resourceType) {
		case kResourceTypeJpg: this->resourceLoader_.textureLoader_ = new JpgLoader; break;
		case kResourceTypePng: this->resourceLoader_.textureLoader_ = new PngLoader; break;
		case kResourceTypeTga: this->resourceLoader_.textureLoader_ = new TgaLoader; break;
		case kResourceTypeBmp: this->resourceLoader_.textureLoader_ = new BmpLoader; break;
		default: break;
		}

		if (!this->resourceLoader_.textureLoader_->Check(File::GetFileExtension(fileName))) {
			PostErrorMessage("Invalid file's extension (%s)", File::GetFileExtension(fileName));
		}

		this->resourceLoader_.textureLoader_->Load(fileName);
	}
	else {
		return LAME_FALSE;
	}

	if (!this->resourceLoader_.audioLoader_) {
		return LAME_FALSE;
	}

	return LAME_TRUE;
}

Bool Resource::Save(StringC fileName) {

	ResourceType resourceType(fileName);
	ResourceFileInfo fileInfo_;

	if (!this->resourceLoader_.audioLoader_) {
		return LAME_FALSE;
	}

	auto resourceLoader_ = this->resourceLoader_;

	if (resourceType != this->resourceLoader_.audioLoader_->GetInfo()->fileType) {

		switch (resourceType) {
		case kResourceTypeJpg: resourceLoader_.textureLoader_
			= new JpgLoader(*(JpgLoaderPtr)this->resourceLoader_.textureLoader_);
			break;
		case kResourceTypePng: resourceLoader_.textureLoader_
			= new PngLoader(*(PngLoaderPtr)this->resourceLoader_.textureLoader_);
			break;
		case kResourceTypeTga: resourceLoader_.textureLoader_
			= new TgaLoader(*(TgaLoaderPtr)this->resourceLoader_.textureLoader_);
			break;
		case kResourceTypeBmp: resourceLoader_.textureLoader_
			= new BmpLoader(*(BmpLoaderPtr)this->resourceLoader_.textureLoader_);
			break;
		case kResourceTypeWav: resourceLoader_.audioLoader_
			= new WavLoader(*(WavLoaderPtr)this->resourceLoader_.textureLoader_);
			break;
		default:
			break;
		}

		if (      resourceLoader_.audioLoader_->GetInfo()->fileType.IsAudio() &&
			this->resourceLoader_.audioLoader_->GetInfo()->fileType.IsTexture()
		) {
			PostErrorMessage("Invalid type conversion from (%s) to (%s)",
				resourceLoader_.audioLoader_->GetInfo()->fileExtension,
				this->resourceLoader_.audioLoader_->GetInfo()->fileExtension);
		}

		if (!resourceType.IsAudio() && !resourceType.IsTexture()) {
			return LAME_FALSE;
		}
	}

	if (resourceType.IsAudio()) {

		if (!this->resourceLoader_.audioLoader_->Check(File::GetFileExtension(fileName))) {
			PostErrorMessage("Invalid file's extension (%s)", File::GetFileExtension(fileName));
		}

		resourceLoader_.audioLoader_->Save(fileName);
	}
	else if (resourceType.IsTexture()) {

		if (!resourceLoader_.textureLoader_->Check(File::GetFileExtension(fileName))) {
			PostErrorMessage("Invalid file's extension (%s)", File::GetFileExtension(fileName));
		}

		resourceLoader_.textureLoader_->Save(fileName);
	}
	else {
		return LAME_FALSE;
	}

	if (resourceType.IsAudio()) {
		delete resourceLoader_.audioLoader_;
	}
	else if (resourceType.IsTexture()) {
		delete resourceLoader_.textureLoader_;
	}

	if (!this->resourceLoader_.audioLoader_) {
		return LAME_FALSE;
	}

	return LAME_TRUE;
}

LAME_END2
