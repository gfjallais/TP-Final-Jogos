cmake_minimum_required(VERSION 3.26)
# Configurando o padrão c++
set(CMAKE_CXX_STANDARD 17)

set(SDL2_DIR "C:/Program Files/SDL/SDL2/cmake")
set(SDL2_image_DIR  "C:/Program Files/SDL/SDL2_image/cmake")
set(SDL2_ttf_DIR  "C:/Program Files/SDL/SDL2_ttf/cmake")
set(SDL2_mixer_DIR  "C:/Program Files/SDL/SDL2_mixer/cmake")

set(PROJECT_NAME "tp-final")
project(${PROJECT_NAME})

find_package(SDL2 2.32.2 EXACT REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

add_executable(${PROJECT_NAME}
        Source/Main.cpp
        Source/Math.cpp
        Source/Random.cpp
        Source/Actors/Actor.cpp
        Source/Components/Component.cpp
        Source/Game.cpp
        Source/Components/DrawComponents/DrawComponent.cpp
        Source/Components/DrawComponents/DrawComponent.h
        Source/Components/RigidBodyComponent.cpp
        Source/Components/RigidBodyComponent.h
        Source/Components/ColliderComponents/CircleColliderComponent.cpp
        Source/Components/ColliderComponents/CircleColliderComponent.h
        Source/Json.h
        Source/CSV.h
        Source/Components/DrawComponents/DrawSpriteComponent.cpp
        Source/Components/DrawComponents/DrawSpriteComponent.h
        Source/Components/DrawComponents/DrawPolygonComponent.cpp
        Source/Components/DrawComponents/DrawPolygonComponent.h
        Source/Actors/Mouse.cpp
        Source/Actors/Mouse.h
        Source/Components/ColliderComponents/AABBColliderComponent.cpp
        Source/Components/ColliderComponents/AABBColliderComponent.h
        Source/Components/DrawComponents/DrawAnimatedComponent.cpp
        Source/Components/DrawComponents/DrawAnimatedComponent.h
        Source/Actors/Block.cpp
        Source/Actors/Block.h
        Source/Actors/Goomba.cpp
        Source/Actors/Goomba.h
        Source/Actors/Cheese.cpp
        Source/Actors/Cheese.h
        Source/AudioSystem.cpp
        Source/AudioSystem.h
        Source/UIElements/UIScreen.h
        Source/UIElements/UIScreen.cpp
        Source/UIElements/UIFont.h
        Source/UIElements/UIFont.cpp
        Source/UIElements/UIButton.cpp
        Source/UIElements/UIButton.h
        Source/UIElements/UIText.cpp
        Source/UIElements/UIText.h
        Source/UIElements/UIElement.cpp
        Source/UIElements/UIElement.h
        Source/HUD.cpp
        Source/UIElements/UIImage.cpp
        Source/UIElements/UIImage.h
        Source/SpatialHashing.cpp
        Source/SpatialHashing.h
        Source/Actors/Exit.h
        Source/Actors/Exit.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf SDL2_mixer::SDL2_mixer)
