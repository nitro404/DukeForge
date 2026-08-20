#include "PalettePAL.h"

#include <ByteBuffer.h>
#include <Utilities/StringUtilities.h>

#include <magic_enum/magic_enum.hpp>
#include <spdlog/spdlog.h>

const std::vector<std::string> PalettePAL::FILE_FORMAT_EXTENSIONS({ "PAL" });
const std::string PalettePAL::FILE_FORMAT_NAME("Microsoft Palette");

PalettePAL::PalettePAL(const std::string & filePath)
	: Palette(std::make_unique<ColourTable>(), filePath)
	, m_version(PAL_VERSION) { }

PalettePAL::PalettePAL(std::unique_ptr<ColourTable> colourTable, std::vector<ColourFlag> colourFlags, uint16_t version, const std::string & filePath)
	: Palette(colourTable != nullptr ? std::move(colourTable) : std::make_unique<ColourTable>(), filePath)
	, m_version(version)
	, m_colourFlags(colourFlags) {
	m_colourFlags.resize(m_colourTables.front()->numberOfColours(), ColourFlag::None);
}

PalettePAL::PalettePAL(const ColourTable & colourTable, std::vector<ColourFlag> colourFlags, uint16_t version, const std::string & filePath)
	: Palette(std::make_unique<ColourTable>(colourTable), filePath)
	, m_version(version)
	, m_colourFlags(colourFlags) {
	m_colourFlags.resize(m_colourTables.front()->numberOfColours(), ColourFlag::None);
}

PalettePAL::PalettePAL(PalettePAL && palette) noexcept
	: Palette(std::move(palette))
	, m_colourFlags(std::move(palette.m_colourFlags))
	, m_version(palette.m_version) { }

PalettePAL::PalettePAL(const PalettePAL & palette)
	: Palette(palette)
	, m_colourFlags(palette.m_colourFlags)
	, m_version(palette.m_version) { }

PalettePAL & PalettePAL::operator = (PalettePAL && palette) noexcept {
	if(this != &palette) {
		Palette::operator = (std::move(palette));

		m_version = palette.m_version;
		m_colourFlags = std::move(palette.m_colourFlags);
	}

	return *this;
}

PalettePAL & PalettePAL::operator = (const PalettePAL & palette) {
	Palette::operator = (palette);

	m_version = palette.m_version;
	m_colourFlags = palette.m_colourFlags;

	return *this;
}

PalettePAL::~PalettePAL() { }

const std::vector<std::string> & PalettePAL::getFileFormatExtensions() const {
	return FILE_FORMAT_EXTENSIONS;
}

const std::string & PalettePAL::getFileFormatName() const {
	return FILE_FORMAT_NAME;
}

uint16_t PalettePAL::getVersion() const {
	return m_version;
}

const std::vector<PalettePAL::ColourFlag> & PalettePAL::getColourFlags() const {
	return m_colourFlags;
}

size_t PalettePAL::getDocumentSizeInBytes() const {
	return getSizeInBytes() - RIFF_SIGNATURE_SIZE_BYTES - sizeof(uint32_t);
}

size_t PalettePAL::getPaletteChunkSizeInBytes() const {
	return (sizeof(uint16_t) * 2) + (m_colourTables.front()->numberOfColours() * BYTES_PER_COLOUR);
}

std::unique_ptr<PalettePAL> PalettePAL::readFrom(const ByteBuffer & byteBuffer) {
	// https://www.cyotek.com/blog/loading-microsoft-riff-palette-pal-files-with-csharp
	// https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-logpalette
	// https://learn.microsoft.com/en-us/previous-versions/dd162769(v=vs.85)

	byteBuffer.setEndianness(ENDIANNESS);

	bool error = false;

	std::string fileSignature(byteBuffer.readString(RIFF_SIGNATURE_SIZE_BYTES, &error));

	if(error) {
		spdlog::error("Invalid Microsoft PAL palette file format, missing Resource Interchange File Format ('{}') signature.", RIFF_SIGNATURE);
		return nullptr;
	}

	if(!Utilities::areStringsEqual(fileSignature, RIFF_SIGNATURE)) {
		spdlog::error("Invalid Microsoft PAL palette file format, invalid Resource Interchange File Format ('{}') signature.", RIFF_SIGNATURE);
		return nullptr;
	}

	uint32_t documentSize = byteBuffer.readUnsignedInteger(&error);

	if(error) {
		spdlog::error("Missing Microsoft PAL palette file format document size value.");
		return nullptr;
	}

	std::string fileFormType(byteBuffer.readString(FORM_TYPE_SIZE_BYTES, &error));

	if(error) {
		spdlog::error("Invalid Microsoft PAL palette file format, missing form type identifier.");
		return nullptr;
	}

	if(!Utilities::areStringsEqual(fileFormType, PAL_FORM_TYPE)) {
		spdlog::error("Invalid Microsoft PAL palette form type '{}', expected '{}'.", fileFormType, PAL_FORM_TYPE);
		return nullptr;
	}

	std::string chunkType(byteBuffer.readString(CHUNK_TYPE_SIZE_BYTES, &error));

	if(error) {
		spdlog::error("Invalid Microsoft PAL palette file format, missing chunk type identifier.");
		return nullptr;
	}

	if(!Utilities::areStringsEqual(chunkType, RGB_CHUNK_TYPE)) {
		spdlog::error("Unsupported Microsoft PAL palette chunk type '{}', expected '{}'.", chunkType, RGB_CHUNK_TYPE);
		return nullptr;
	}

	uint32_t chunkSize = byteBuffer.readUnsignedInteger(&error);

	if(error) {
		spdlog::error("Missing Microsoft PAL palette file '{}' chunk size.", chunkType);
		return nullptr;
	}

	uint16_t version = byteBuffer.readUnsignedShort(&error);

	if(error) {
		spdlog::error("Missing Microsoft PAL version.");
		return nullptr;
	}

	if(version != PAL_VERSION) {
		spdlog::error("Unsupported Microsoft PAL version {}, only version {} is supported.", version, PAL_VERSION);
		return nullptr;
	}

	uint16_t numberOfColours = byteBuffer.readUnsignedShort(&error);

	if(error) {
		spdlog::error("Missing Microsoft PAL data size.");
		return nullptr;
	}

	std::vector<Colour> colours;
	colours.reserve(numberOfColours);
	std::vector<ColourFlag> colourFlags;
	colourFlags.reserve(numberOfColours);

	for(uint16_t i = 0; i < numberOfColours; i++) {
		colours.emplace_back(Colour::readFrom(byteBuffer, false, &error));

		if(error) {
			spdlog::error("Failed to read Microsoft PAL palette colour #{} value from binary data.", i + 1);
			return nullptr;
		}

		uint8_t colourFlagValue = byteBuffer.readUnsignedByte(&error);

		if(error) {
			spdlog::error("Failed to read Microsoft PAL palette colour #{} flag from binary data.", i + 1);
			return nullptr;
		}

		std::optional<ColourFlag> optionalColourFlag(magic_enum::enum_cast<ColourFlag>(colourFlagValue));

		if(!optionalColourFlag.has_value()) {
			spdlog::error("Invalid Microsoft PAL palette colour #{} flag: {}.", i + 1, colourFlagValue);
			return nullptr;
		}

		colourFlags.push_back(optionalColourFlag.value());
	}

	size_t numberOfAdditionalBytes = byteBuffer.numberOfBytesRemaining();

	if(numberOfAdditionalBytes != 0) {
		spdlog::warn("Microsoft PAL binary data contains an additional {} trailing byte{} which will be discarded.", numberOfAdditionalBytes, numberOfAdditionalBytes == 1 ? "" : "s");
	}

	return std::make_unique<PalettePAL>(colours, colourFlags, version);
}

std::unique_ptr<PalettePAL> PalettePAL::loadFrom(const std::string & filePath) {
	std::unique_ptr<ByteBuffer> paletteData(ByteBuffer::readFrom(filePath));

	if(paletteData == nullptr) {
		spdlog::error("Failed to read Microsoft PAL palette binary data from file: '{}'.", filePath);
		return nullptr;
	}

	std::unique_ptr<PalettePAL> palette(PalettePAL::readFrom(*paletteData));

	if(palette == nullptr) {
		spdlog::error("Failed to parse Microsoft PAL palette binary data from file contents: '{}'.", filePath);
		return nullptr;
	}

	palette->setFilePath(filePath);

	return palette;
}

bool PalettePAL::writeTo(ByteBuffer & byteBuffer) const {
	byteBuffer.setEndianness(ENDIANNESS);

	if(!byteBuffer.writeString(RIFF_SIGNATURE)) {
		return false;
	}

	if(!byteBuffer.writeUnsignedInteger(getDocumentSizeInBytes())) {
		return false;
	}

	if(!byteBuffer.writeString(PAL_FORM_TYPE)) {
		return false;
	}

	if(!byteBuffer.writeString(RGB_CHUNK_TYPE)) {
		return false;
	}

	if(!byteBuffer.writeUnsignedInteger(getPaletteChunkSizeInBytes())) {
		return false;
	}

	if(!byteBuffer.writeUnsignedShort(m_version)) {
		return false;
	}

	std::shared_ptr<ColourTable> colourTable(m_colourTables.front());

	if(!byteBuffer.writeUnsignedShort(colourTable->numberOfColours())) {
		return false;
	}

	for(size_t i = 0; i < colourTable->numberOfColours(); i++) {
		if(!(*colourTable)[i].writeTo(byteBuffer, false)) {
			return false;
		}

		if(!byteBuffer.writeUnsignedByte(magic_enum::enum_integer(m_colourFlags[i]))) {
			return false;
		}
	}

	return true;
}

void PalettePAL::addMetadata(std::vector<std::pair<std::string, std::string>> & metadata) const {
	Palette::addMetadata(metadata);

	metadata.push_back({ "Version", std::to_string(m_version) });
	metadata.push_back({ "Document Size", std::to_string(getDocumentSizeInBytes()) });
	metadata.push_back({ "Palette Chunk Size", std::to_string(getPaletteChunkSizeInBytes()) });
}

Endianness PalettePAL::getEndianness() const {
	return ENDIANNESS;
}

size_t PalettePAL::getSizeInBytes() const {
	return RIFF_SIGNATURE_SIZE_BYTES + sizeof(uint32_t) + FORM_TYPE_SIZE_BYTES + CHUNK_TYPE_SIZE_BYTES + sizeof(uint32_t) + (sizeof(uint16_t) * 2) + (m_colourTables.front()->numberOfColours() * BYTES_PER_COLOUR);
}

bool PalettePAL::isValid(bool verifyParent) const {
	if(!Palette::isValid(verifyParent)) {
		return false;
	}

	return m_colourTables.front()->numberOfColours() == m_colourFlags.size();
}

bool PalettePAL::operator == (const PalettePAL & palette) const {
	return Palette::operator == (palette) &&
		   m_version == palette.m_version &&
		   m_colourFlags == palette.m_colourFlags;
}

bool PalettePAL::operator != (const PalettePAL & palette) const {
	return !operator == (palette);
}
