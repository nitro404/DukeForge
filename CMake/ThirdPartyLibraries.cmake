include_guard()

hunter_add_package(CSSColorParser)
hunter_add_package(Expat)
hunter_add_package(gcem)
hunter_add_package(JDKSMIDI)
hunter_add_package(SndFile)

find_package(CSSColorParser CONFIG REQUIRED)
find_package(Expat CONFIG REQUIRED)
find_package(gcem CONFIG REQUIRED)
find_package(JDKSMIDI CONFIG REQUIRED)
find_package(SndFile CONFIG REQUIRED)
