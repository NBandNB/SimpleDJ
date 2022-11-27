set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


set(include_dir ${CMAKE_SOURCE_DIR}/include)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Multimedia
        REQUIRED)

qt_add_executable(SimpleDJ
        main.cpp
        ${include_dir}/MainWindow.h
        ${include_dir}/CurrentSongWidget.h
        ${include_dir}/SongQueueItemWidget.h
        ${include_dir}/SongQueueWidget.h
        ${include_dir}/SongSearchItemWidget.h
        ${include_dir}/SongSearchWidget.h
        ${include_dir}/Song.h
        ${include_dir}/SongLoader.h
        ${include_dir}/QueueLayout.h
        ${include_dir}/ChangePinPopup.h
        ${include_dir}/UnlockPopup.h
        MainWindow.cpp
        CurrentSongWidget.cpp
        Song.cpp
        SongQueueItemWidget.cpp
        SongQueueWidget.cpp
        SongSearchItemWidget.cpp
        SongSearchWidget.cpp
        SongLoader.cpp
        QueueLayout.cpp
        ChangePinPopup.cpp
        UnlockPopup.cpp)

target_link_libraries(SimpleDJ PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Multimedia
        libcurl
        wolfssl
        )