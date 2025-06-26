set(ENGINE_ROOT_DIR "${FALCON_ROOT_DIR}/engine")
set(THIRD_PARTY_DIR "${ENGINE_ROOT_DIR}/3rdparty")
set(ENGINE_ASSET_DIR "/asset")

include_directories(
${THIRD_PARTY_DIR}
)
include_directories(${FALCON_ROOT_DIR})
add_subdirectory(3rdparty)
add_subdirectory(runtime)



