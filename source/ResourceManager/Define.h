#ifndef __LAME_RESOURCE_MANAGER__DEFINE__
#define __LAME_RESOURCE_MANAGER__DEFINE__

#include <Core/Core.h>

#if defined(LAME_API)
#  undef LAME_API
#endif

#if defined(LAME_MSVC)
#  if defined(RESOURCEMANAGER_EXPORTS)
#    define LAME_API __declspec(dllexport)
#  else
#    define LAME_API __declspec(dllimport)
#  endif
#else
#  define LAME_API
#endif

LAME_BEGIN2(ResourceManager)

typedef class ResourceController *ResourceControllerPtr;

typedef enum {
	kResourceTypeDefault,
	kResourceTypeJpg,
	kResourceTypePng,
	kResourceTypeTga,
	kResourceTypeBmp,
	kResourceTypeWav
} ResourceTypeID;

typedef class ResourceType {
public:
	ResourceType() :
		resourceType_(kResourceTypeDefault)
	{
	}
	ResourceType(ResourceTypeID resourceType) :
		resourceType_(resourceType)
	{
	}
public:
	ResourceType(StringC fileName) {
		if (fileName) {
			this->ParseFileName(fileName);
		}
	}
public:
	inline ResourceTypeID GetID(Void) const { return this->resourceType_; }
	inline ResourceTypeID SetID(ResourceTypeID resourceType) { return (this->resourceType_ = resourceType); }
public:
	inline operator ResourceTypeID(Void) const { return this->resourceType_; }
	inline ResourceTypeID operator = (ResourceTypeID resourceType) { return (this->resourceType_ = resourceType); }
public:
	inline Bool IsTexture() {
		return
			this->resourceType_ == kResourceTypeJpg ||
			this->resourceType_ == kResourceTypePng ||
			this->resourceType_ == kResourceTypeTga ||
			this->resourceType_ == kResourceTypeBmp;
	}
	inline Bool IsAudio() {
		return
			this->resourceType_ == kResourceTypeWav;
	}
public:
	ResourceTypeID ParseFileName(StringC fileName) {

		Core::FilePathInfo filePathInfo(fileName);

		if (filePathInfo.extension == "jpg") {
			this->SetID(kResourceTypeJpg);
		} else if (filePathInfo.extension == "png") {
			this->SetID(kResourceTypePng);
		} else if (filePathInfo.extension == "tga") {
			this->SetID(kResourceTypeTga);
		} else if (filePathInfo.extension == "bmp") {
			this->SetID(kResourceTypeBmp);
		} else if (filePathInfo.extension == "wav") {
			this->SetID(kResourceTypeWav);
		}

		return this->GetID();
	}
private:
	ResourceTypeID resourceType_;
} *ResourceTypePtr;

typedef struct ResourceFileInfo {
public:
	FileName fileName;
	FilePath filePath;
	Uint32 fileSize;
	FileExtension fileExtension;
	ResourceType fileType;
public:
	ResourceFileInfo() {

		*this->fileName = 0;
		*this->filePath = 0;
		*this->fileExtension = 0;

		this->fileSize = 0;
		this->fileType = 0;
	}
public:
	Void BuildFileInfo(StringC fileName) {

		Core::FilePathInfo filePathInfo(fileName);

		strcpy(this->fileName, filePathInfo.name.data());
		strcpy(this->filePath, filePathInfo.path.data());
		strcpy(this->fileExtension, filePathInfo.extension.data());

		this->fileSize = Core::File::GetFileSize(fileName);
	}
} *ResourceFileInfoPtr;

typedef struct ResourceTextureInfo : ResourceFileInfo {

	Uint16 imageWidth;
	Uint16 imageHeight;
	Uint16 imageBits;
	Uint16 imageBytes;
	Uint32 imageSize;
	VoidP imageData;
	Uint32 imageJpegQuality;

	ResourceTextureInfo() {

		this->imageBits = 0;
		this->imageBytes = 0;
		this->imageData = 0;
		this->imageHeight = 0;
		this->imageJpegQuality = 0;
		this->imageSize = 0;
		this->imageWidth = 0;
	}

} *ResourceTextureInfoPtr;

typedef struct ResourceAudioInfo : ResourceFileInfo {

	Uint32 audioLength;
	VoidP audioData;
	Uint32 audioChunkSize;
	Uint16 audioFormatType;
	Uint32 audioChannels;
	Uint32 audioSampleRate;
	Uint32 audioAvgBytesPerSec;
	Uint16 audioBytesPerSample;
	Uint16 audioBitsPerSample;
	Uint32 audioDataSize;

	ResourceAudioInfo() {

		this->audioAvgBytesPerSec = 0;
		this->audioBitsPerSample = 0;
		this->audioBytesPerSample = 0;
		this->audioChannels = 0;
		this->audioChunkSize = 0;
		this->audioData = 0;
		this->audioFormatType = 0;
		this->audioLength = 0;
		this->audioSampleRate = 0;
	}

} *ResourceAudioInfoPtr;

typedef Core::Exception ResourceException;

LAME_END2

#endif // ~__LAME_RESOURCE_MANAGER__DEFINE__