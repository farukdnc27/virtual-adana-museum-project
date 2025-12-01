cmake_minimum_required(VERSION 3.10)

# opengl 
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.10")
    cmake_policy(SET CMP0072 NEW)
endif()

project(VirtualAdanaMuseum)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Bağımlılık dizinlerini tanımla
set(LINKING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Linking)
set(INCLUDE_DIR ${LINKING_DIR}/include)
set(LIB_DIR ${LINKING_DIR}/lib)

# OpenGL'yi bul bu kısımda hata aldığım için daha fzla tarama ekledim
if(WIN32)
    find_package(OpenGL REQUIRED)
else()
    # Linux ve macOS için PkgConfig opsiyonel
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(OPENGL REQUIRED gl)
    else()
        find_package(OpenGL REQUIRED)
    endif()
endif()

# Include dizinlerini ekle tektek tanıtıyoruz 
include_directories(
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/glm
    ${INCLUDE_DIR}/GLFW
    ${INCLUDE_DIR}/glad
    ${INCLUDE_DIR}/assimp
    ${INCLUDE_DIR}/stb
    ${INCLUDE_DIR}/KHR
    ${LINKING_DIR}/imgui
)

# Kaynak dosyaları
set(SOURCES
    main.cpp
    Camera.cpp
    Shader.cpp
    WindowManager.cpp
    InputManager.cpp
    SceneManager.cpp
    ImGuiManager.cpp
    Robot.cpp
    Light.cpp
    Texture.cpp
    ResourceManager.cpp
    MuseumObject.cpp
    MuseumArtifact.cpp
    glad.c
)

# Header dosyaları
set(HEADERS
    Camera.h
    Shader.h
    WindowManager.h
    InputManager.h
    SceneManager.h
    ImGuiManager.h
    Robot.h
    Light.h
    Texture.h
    ResourceManager.h
    MuseumObject.h
    MuseumArtifact.h
    Frustum.h
    ShaderSetup.h
)

# ImGui kaynak dosyaları
set(IMGUI_SOURCES
    ${LINKING_DIR}/imgui/imgui.cpp
    ${LINKING_DIR}/imgui/imgui_demo.cpp
    ${LINKING_DIR}/imgui/imgui_draw.cpp
    ${LINKING_DIR}/imgui/imgui_tables.cpp
    ${LINKING_DIR}/imgui/imgui_widgets.cpp
    ${LINKING_DIR}/imgui/imgui_impl_glfw.cpp
    ${LINKING_DIR}/imgui/imgui_impl_opengl3.cpp
)

# Executable oluştur
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${IMGUI_SOURCES})

# Kütüphaneleri bağlama konusunda hata aldık burası rebuild edildi tekrar yazdım
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        OpenGL::GL
        ${LIB_DIR}/GLFW/glfw3.lib
        ${LIB_DIR}/assimp/assimp-vc143-mtd.lib
    )
    
    # DLL'leri PowerShell ile kopyala bu ksıım ai önerdi
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND powershell -Command "Copy-Item '${CMAKE_CURRENT_SOURCE_DIR}/glfw3.dll' '$<TARGET_FILE_DIR:${PROJECT_NAME}>' -Force"
        COMMAND powershell -Command "Copy-Item '${CMAKE_CURRENT_SOURCE_DIR}/assimp-vc143-mtd.dll' '$<TARGET_FILE_DIR:${PROJECT_NAME}>' -Force"
    )
elseif(UNIX AND NOT APPLE)
    # Linux için önce sistem de glfw varsa onu kullan extra hata mesajı ekledim
    find_package(glfw3 QUIET)
    
    if(glfw3_FOUND)
        message(STATUS "Sistem GLFW'si bulundu")
        target_link_libraries(${PROJECT_NAME} PRIVATE
            ${OPENGL_LIBRARIES}
            glfw
            dl
            pthread
        )
    else()
        message(STATUS "Sistem GLFW'si bulunamadı, manuel arama yapılıyor")
        
        # Manuel GLFW arama - daha kapsamlı yollar
        set(GLFW_POSSIBLE_PATHS
            ${LIB_DIR}/GLFW/libglfw.so.3.3
            ${LIB_DIR}/GLFW/libglfw.so.3
            ${LIB_DIR}/GLFW/libglfw.so
            /usr/lib/x86_64-linux-gnu/libglfw.so.3
            /usr/lib/x86_64-linux-gnu/libglfw.so
            /usr/lib/libglfw.so.3
            /usr/lib/libglfw.so
            /usr/local/lib/libglfw.so.3
            /usr/local/lib/libglfw.so
        )
        
        find_library(GLFW_LIBRARY
            NAMES glfw glfw3
            PATHS ${GLFW_POSSIBLE_PATHS}
        )
        
        if(GLFW_LIBRARY)
            message(STATUS "GLFW bulundu: ${GLFW_LIBRARY}")
            target_link_libraries(${PROJECT_NAME} PRIVATE
                ${OPENGL_LIBRARIES}
                ${GLFW_LIBRARY}
                dl
                pthread
            )
            
            # GLFW dosyasını kopyala proje yolundan
            get_filename_component(GLFW_FILENAME ${GLFW_LIBRARY} NAME)
            if(${GLFW_LIBRARY} MATCHES "${CMAKE_CURRENT_SOURCE_DIR}")
                add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${GLFW_LIBRARY}"
                        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${GLFW_FILENAME}"
                )
            endif()
        else()
            message(FATAL_ERROR "GLFW kütüphanesi bulunamadı! Lütfen şu komut ile yükleyin: sudo apt install libglfw3-dev")
        endif()
    endif()
    
    # Assimp kütüphanesini kontrol et ve bağla
    # Önce diğer kısımda oludu gibi sistemdeki  assimpi denedim ev hata mesajı ekledim
    find_package(assimp QUIET)
    if(assimp_FOUND)
        message(STATUS "Sistem Assimp bulundu")
        target_link_libraries(${PROJECT_NAME} PRIVATE assimp)
    else()
        # Manuel assimp arama
        set(ASSIMP_POSSIBLE_PATHS
            ${LIB_DIR}/assimp/libassimp.so.5
            ${LIB_DIR}/assimp/libassimp.so
            /usr/lib/x86_64-linux-gnu/libassimp.so.5
            /usr/lib/x86_64-linux-gnu/libassimp.so
            /usr/lib/libassimp.so.5
            /usr/lib/libassimp.so
            /usr/local/lib/libassimp.so.5
            /usr/local/lib/libassimp.so
        )
        
        find_library(ASSIMP_LIBRARY
            NAMES assimp assimp.5
            PATHS ${ASSIMP_POSSIBLE_PATHS}
        )
        
        if(ASSIMP_LIBRARY)
            message(STATUS "Assimp bulundu: ${ASSIMP_LIBRARY}")
            target_link_libraries(${PROJECT_NAME} PRIVATE ${ASSIMP_LIBRARY})
            
            # Assimp dosyasını kopyala (eğer proje dizininden geliyorsa)
            get_filename_component(ASSIMP_FILENAME ${ASSIMP_LIBRARY} NAME)
            if(${ASSIMP_LIBRARY} MATCHES "${CMAKE_CURRENT_SOURCE_DIR}")
                add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${ASSIMP_LIBRARY}"
                        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${ASSIMP_FILENAME}"
                )
            endif()
        else()
            message(FATAL_ERROR "Assimp kütüphanesi bulunamadı! Lütfen şu komut ile yükleyin: sudo apt install libassimp-dev")
        endif()
    endif()

elseif(APPLE)
    # macOS da çalışmıyor belkide ben çalıştıramadım ama genede burda duruyor m1 işlemcili cihazda denedim olmadı 
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${OPENGL_LIBRARIES}
        ${LIB_DIR}/GLFW/libglfw.dylib
        ${LIB_DIR}/assimp/libassimp.dylib
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()

# Shader ve model dosyalarını kopyala
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    # Shader dosyalarını kopyala
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
    
    # Model dosyalarını kopyala
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/models"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/models"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/models"
)

# Debug bilgilerini yazdır
message(STATUS "Proje dizini: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Include dizini: ${INCLUDE_DIR}")
message(STATUS "Lib dizini: ${LIB_DIR}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")