include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.90
    GIT_PROGRESS TRUE
)
message("ImGui")
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)

    file(GLOB SRC_IMGUI
        "${imgui_SOURCE_DIR}/*.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp"
        "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"
    )

    # Periksa sama ada fail lynnncrash.cpp wujud sebelum memasukkannya
    if(EXISTS "${CMAKE_SOURCE_DIR}/src/backend/commands/player/crashes/lynnncrash.cpp")
        list(APPEND SRC_IMGUI "${CMAKE_SOURCE_DIR}/src/backend/commands/player/crashes/lynnncrash.cpp")
    else()
        message(WARNING "lynnncrash.cpp tidak ditemui! Sila pastikan fail ini wujud.")
    endif()

    add_library(imgui STATIC ${SRC_IMGUI})

    # Source group hanya untuk fail imgui
    source_group(TREE ${imgui_SOURCE_DIR} PREFIX "imgui" FILES
        "${imgui_SOURCE_DIR}/*.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp"
        "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"
    )

    # Kumpulkan fail yang tidak berada dalam direktori imgui
    if(EXISTS "${CMAKE_SOURCE_DIR}/src/backend/commands/player/crashes/lynnncrash.cpp")
        source_group("Crashes" FILES "${CMAKE_SOURCE_DIR}/src/backend/commands/player/crashes/lynnncrash.cpp")
    endif()

    target_include_directories(imgui PRIVATE
        "${imgui_SOURCE_DIR}"
        "${imgui_SOURCE_DIR}/backends"
        "${imgui_SOURCE_DIR}/misc/cpp"
    )
endif()
set_property(TARGET imgui PROPERTY CXX_STANDARD 23)


