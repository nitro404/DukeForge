#ifndef _PALETTE_H_
#define _PALETTE_H_

#include "../GameFile.h"
#include "ColourTable.h"

#include <Colour.h>

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

class Palette : public GameFile {
public:
	Palette(Palette && palette) noexcept;
	Palette(const Palette & palette);
	Palette & operator = (Palette && palette) noexcept;
	Palette & operator = (const Palette & palette);
	~Palette() override;

	bool hasTransparentColourIndex(uint8_t colourTableIndex = 0) const;
	std::optional<uint8_t> getTransparentColourIndex(uint8_t colourTableIndex = 0) const;
	bool setTransparentColourIndex(uint8_t transparentColourIndex, uint8_t colourTableIndex = 0) const;
	bool clearTransparentColourIndex(uint8_t colourTableIndex = 0) const;
	std::optional<uint16_t> numberOfColours(uint8_t colourTableIndex = 0) const;
	bool setNumberOfColours(uint16_t colourCount, uint8_t colourTableIndex = 0) const;
	const Colour & getColour(uint8_t colourIndex, uint8_t colourTableIndex = 0, bool * error = nullptr) const;
	std::optional<Colour> getColour(uint8_t colourIndex, uint8_t colourTableIndex = 0) const;
	const Colour & getColour(uint8_t x, uint8_t y, uint8_t colourTableIndex = 0, bool * error = nullptr) const;
	std::optional<Colour> getColour(uint8_t x, uint8_t y, uint8_t colourTableIndex = 0) const;
	bool setColour(uint8_t x, uint8_t y, const Colour & colour, uint8_t colourTableIndex = 0);
	bool setColour(uint8_t colourIndex, const Colour & colour, uint8_t colourTableIndex = 0);
	std::shared_ptr<ColourTable> getColourTable(uint8_t colourTableIndex = 0) const;
	const std::vector<std::shared_ptr<ColourTable>> & getAllColourTables() const;
	bool updateColourTable(uint8_t colourTableIndex, const ColourTable & colourTable);
	bool updateAllColourTables(const std::vector<std::shared_ptr<const ColourTable>> & colourTables);
	bool fillColourTableWithColour(const Colour & colour, uint8_t colourTableIndex = 0);
	bool fillAllColourTablesWithColour(const Colour & colour);
	uint8_t numberOfColourTables() const;
	const std::string & getColourTableName(uint8_t colourTableIndex = 0) const;
	static bool isValid(const Palette * palette, bool verifyParent = true);

	bool operator == (const Palette & palette) const;
	bool operator != (const Palette & palette) const;

	// GameFile Virtuals
	void addMetadata(std::vector<std::pair<std::string, std::string>> & metadata) const override;
	bool isValid(bool verifyParent = true) const override;

	static constexpr uint8_t PALETTE_WIDTH = 16;
	static constexpr uint8_t PALETTE_HEIGHT = 16;

protected:
	Palette(const std::string & filePath);
	Palette(std::unique_ptr<ColourTable> colourTable = nullptr, const std::string & filePath = {});
	Palette(std::vector<std::unique_ptr<ColourTable>> && colourTables, const std::string & filePath = {});

	// GameFile Virtruals
	void setModified(bool modified) const override;

	std::vector<std::shared_ptr<ColourTable>> m_colourTables;

private:
	void onColourTableModified(const ColourTable & colourTable);
	void connectSignals();
	void updateParent();

	std::vector<boost::signals2::connection> m_colourTableConnections;
};

#endif // _PALETTE_H_
