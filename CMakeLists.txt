cmake_minimum_required(VERSION 3.23)

add_subdirectory(libraries EXCLUDE_FROM_ALL)

project(SimpleDJ)

set(CMAKE_CXX_STANDARD 23)

include_directories(include)

add_subdirectory(src)

set(CPACK_PACKAGE_VENDOR "LotsOfBlochs")
set(CPACK_CREATE_DESKTOP_LINKS SimpleDJ)
set(CPACK_PACKAGE_EXECUTABLES "SimpleDJ" "SimpleDJ")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.md")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_STRIP_FILES TRUE)

if(WIN32)
    set(CPACK_GENERATOR "STGZ;ZIP;NSIS")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_PACKAGING_INSTALL_PREFIX "/")

    set(MACOSX_BUNDLE_BUNDLE_NAME ${CPACK_PACKAGE_NAME})
else()
    set(CPACK_GENERATOR "DEB;TBZ2;TXZ")
endif()

include(CPack)