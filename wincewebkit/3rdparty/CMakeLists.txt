SET(3RDPARTY_DIR "${CMAKE_SOURCE_DIR}/3rdparty")

SET(BUILD_SHARED_LIBS 0)
SET(PNG_SHARED 0)

ADD_SUBDIRECTORY(libjpeg)
ADD_SUBDIRECTORY(libpng)
ADD_SUBDIRECTORY(libxml2)
ADD_SUBDIRECTORY(zlib)


INCLUDE_IF_EXISTS(${3RDPARTY_DIR}/CMakeLists${PORT}.txt)