#ifndef _FILE_METADATA_PROVIDER_H_
#define _FILE_METADATA_PROVIDER_H_

#include <Endianness.h>

#include <boost/signals2.hpp>

#include <memory>
#include <string>
#include <vector>

class ByteBuffer;

class GameFile {
public:
	GameFile(GameFile && gameFile) noexcept;
	GameFile(const GameFile & gameFile);
	GameFile & operator = (GameFile && gameFile) noexcept;
	GameFile & operator = (const GameFile & gameFile);
	virtual ~GameFile();

	const std::string & getDefaultFileFormatExtension() const;
	virtual const std::vector<std::string> & getFileFormatExtensions() const = 0;
	virtual const std::string & getFileFormatName() const = 0;
	bool hasFilePath() const;
	const std::string & getFilePath() const;
	std::string_view getFileName() const;
	std::string_view getFileExtension() const;
	void setFilePath(const std::string & filePath);
	void clearFilePath();
	bool isModified() const;
	std::unique_ptr<ByteBuffer> getData() const;
	virtual bool writeTo(ByteBuffer & byteBuffer) const = 0;
	virtual bool saveTo(const std::string & filePath, bool overwrite = true) const;
	bool save(bool overwrite = true);
	std::vector<std::pair<std::string, std::string>> getMetadata() const;
	virtual void addMetadata(std::vector<std::pair<std::string, std::string>> & metadata) const = 0;
	virtual Endianness getEndianness() const = 0;
	virtual size_t getSizeInBytes() const = 0;
	virtual bool isValid(bool verifyParent = true) const = 0;
	static bool isValid(const GameFile * gameFile, bool verifyParent = true);

	boost::signals2::signal<void (const GameFile & /* gameFile */)> modified;

protected:
	GameFile(const std::string & filePath = {});

	virtual void setModified(bool modified) const;

	std::string m_filePath;
	mutable bool m_modified;
};

#endif // _FILE_METADATA_PROVIDER_H_
