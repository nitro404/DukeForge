include_guard()

# CatchSelfTest is used by wxWidgets.
hunter_config(CatchSelfTest
	URL "https://codeload.github.com/catchorg/catch2/zip/tags/v3.5.0"
	VERSION "3.5.0.0"
	SHA1 "fd37a71ebf9455181a44fe7e2a0d56bb3c07b973"
	CMAKE_ARGS
		CATCH_INSTALL_DOCS=OFF
		CATCH_INSTALL_EXTRAS=OFF
		CATCH_DEVELOPMENT_BUILD=OFF
		CATCH_ENABLE_REPRODUCIBLE_BUILD=ON
		CATCH_BUILD_TESTING=OFF
		CATCH_BUILD_EXAMPLES=OFF
		CATCH_BUILD_EXTRA_TESTS=OFF
		CATCH_BUILD_FUZZERS=OFF
		CATCH_BUILD_BENCHMARKS=OFF
		CATCH_ENABLE_COVERAGE=OFF
		CATCH_ENABLE_WERROR=ON
		CATCH_BUILD_SURROGATES=OFF
		CATCH_ENABLE_CONFIGURE_TESTS=OFF
		CATCH_ENABLE_CMAKE_HELPER_TESTS=OFF
)

hunter_config(CSSColorParser
	URL "https://github.com/Telefrag-Software/css-color-cpp/archive/refs/heads/hunter-1.0.0.zip"
	VERSION "1.0.0"
	SHA1 "010e7260297e8091d4e97005ebd680eb1cac665b"
)

# LibEXPAT is used by wxWidgets.
hunter_config(Expat
	URL "https://codeload.github.com/libexpat/libexpat/zip/tags/R_2_7_3"
	VERSION "2.7.3.0"
	SHA1 "af45665222bb5c749a0750e971f2f72839f8999c"
	CMAKE_ARGS
		EXPAT_BUILD_TOOLS=OFF
		EXPAT_BUILD_EXAMPLES=OFF
		EXPAT_BUILD_TESTS=OFF
		EXPAT_SHARED_LIBS=OFF
		EXPAT_BUILD_DOCS=OFF
		EXPAT_BUILD_FUZZERS=OFF
		EXPAT_BUILD_PKGCONFIG=ON
		EXPAT_OSSFUZZ_BUILD=OFF
		EXPAT_ENABLE_INSTALL=ON
		EXPAT_DTD=ON
		EXPAT_GE=ON
		EXPAT_NS=ON
		EXPAT_WARNINGS_AS_ERRORS=OFF
		EXPAT_ATTR_INFO=OFF
		EXPAT_LARGE_SIZE=OFF
		EXPAT_MIN_SIZE=OFF
		EXPAT_MSVC_STATIC_CRT=ON
)

# GCE-Math is used by CSSColorParser.
hunter_config(gcem
	URL "https://github.com/Telefrag-Software/gcem/archive/refs/heads/hunter-1.18.0.1.zip"
	VERSION "1.18.0.1"
	SHA1 "b74f598acefaf4399becc10bb4b4330183d81692"
)

hunter_config(JDKSMIDI
	URL "https://github.com/Telefrag-Software/jdksmidi/archive/refs/heads/hunter-2014.08.11a.zip"
	VERSION "2014.08.11a"
	SHA1 "2f47cc9b8eb1ef540aa5631501d88231c7b3b8ab"
)

# LibJPEG is used by LibTIFF and wxWidgets.
hunter_config(Jpeg
	URL "https://codeload.github.com/Telefrag-Software/libjpeg/zip/tags/9f"
	VERSION "9f"
	SHA1 "983e8b6e4a508944522f48c2dd680331012a322f"
)

hunter_config(SndFile
	URL "https://codeload.github.com/Telefrag-Software/libsndfile/zip/tags/1.2.2.1"
	VERSION "1.2.2.1"
	SHA1 "92258be67894f09f55f91cfa0d19e0957271a9dc"
	CMAKE_ARGS
		BUILD_TESTING=OFF
		INSTALL_MANPAGES=OFF
		ENABLE_CPACK=OFF
)
