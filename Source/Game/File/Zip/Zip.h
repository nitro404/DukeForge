#ifndef _ZIP_H_
#define _ZIP_H_

#include "../GameFile.h"

#include <ByteBuffer.h>
#include <Endianness.h>

#include <memory>
#include <string>

class ZipArchive;

class Zip final : public GameFile {
public:
	Zip(const std::string & filePath = {});
	Zip(std::unique_ptr<ZipArchive> zipArchive);
	Zip(Zip && zip) noexcept;
	Zip(const Zip & zip);
	Zip & operator = (Zip && zip) noexcept;
	Zip & operator = (const Zip & zip);
	~Zip() override;

	static std::unique_ptr<Zip> readFrom(const ByteBuffer & byteBuffer);
	static std::unique_ptr<Zip> loadFrom(const std::string & filePath);

	// GameFile Virtuals
	const std::vector<std::string> & getFileFormatExtensions() const override;
	const std::string & getFileFormatName() const override;
	bool writeTo(ByteBuffer & byteBuffer) const override;
	void addMetadata(std::vector<std::pair<std::string, std::string>> & metadata) const override;
	Endianness getEndianness() const override;
	size_t getSizeInBytes() const override;
	bool isValid(bool verifyParent = true) const override;

	static const std::vector<std::string> FILE_FORMAT_EXTENSIONS;
	static const std::string FILE_FORMAT_NAME;

private:
	std::unique_ptr<ZipArchive> m_zipArchive;
};

#endif // _ZIP_H_
