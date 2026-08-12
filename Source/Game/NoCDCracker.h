#ifndef _NO_CD_CRACKER_H_
#define _NO_CD_CRACKER_H_

#include <BitmaskOperators.h>
#include <ByteBuffer.h>

#include <string>

class NoCDCracker final {
public:
	enum class GameExecutableStatus
	{
		Missing = 0,
		Exists = 1,
		Invalid = 1 << 1,
		RegularVersion = 1 << 2,
		PlutoniumPak = 1 << 3,
		AtomicEdition = 1 << 4,
		SpecialEdition = 1 << 5,
		Cracked = 1 << 6
	};

	static GameExecutableStatus getGameExecutableStatus(const std::string & gameExecutablePath);
	static GameExecutableStatus getGameExecutableStatus(const ByteBuffer * gameExecutableBuffer);
	static bool isRegularVersionGameExecutable(const std::string & gameExecutablePath);
	static bool isPlutoniumPakGameExecutable(const std::string & gameExecutablePath);
	static bool isAtomicEditionGameExecutable(const std::string & gameExecutablePath);
	static bool isAtomicEditionSpecialEditionGameExecutable(const std::string & gameExecutablePath);
	static bool isGameExecutableCrackable(const std::string & gameExecutablePath);
	static bool isGameExecutableCracked(const std::string & gameExecutablePath);
	static bool crackGameExecutable(const std::string & gameExecutablePath, bool writeCDROMFile = true);
	static bool crackGameExecutable(const std::string & inputGameExecutablePath, const std::string & outputGameExecutablePath, bool writeCDROMFile = true);

	static inline const std::string BETA_VERSION_GAME_EXECUTABLE_SHA1 = "69b0efe8963d2039240a257662dd1cac0748bc77";
	static inline const std::string REGULAR_VERSION_GAME_EXECUTABLE_SHA1 = "a64cc5b61cba728427cfcc537aa2f74438ea4c65";
	static inline const std::string PLUTONIUM_PAK_GAME_EXECTUABLE_UNCRACKED_SHA1 = "772d922b16f7b0b11305f1aa7a3fcbb534f884d1";
	static inline const std::string PLUTONIUM_PAK_GAME_EXECTUABLE_CRACKED_SHA1 = "b440dd343df1ce318a03991a056db95a43d5d30c";
	static inline const std::string ATOMIC_EDITION_GAME_EXECTUABLE_UNCRACKED_SHA1 = "f0dc7f1ca810aa517fcad544a3bf5af623a3e44e";
	static inline const std::string ATOMIC_EDITION_GAME_EXECTUABLE_CRACKED_SHA1 = "a849e1e00ac58c0271498dd302d5c5f2819ab2e9";
	static inline const std::string ATOMIC_EDITION_SPECIAL_EDITION_GAME_EXECTUABLE_UNCRACKED_SHA1 = "c0bffb4c31eef79b9f0cf43c3a1a17f2b37ac8cb";
	static inline const std::string ATOMIC_EDITION_SPECIAL_EDITION_GAME_EXECTUABLE_CRACKED_SHA1 = "36f26c3e28bbf72560a72bc3e1b014a35f439c8d";
};

template<>
struct BitmaskOperators<NoCDCracker::GameExecutableStatus> {
	static const bool enabled = true;
};

#endif // _NO_CD_CRACKER_H_
