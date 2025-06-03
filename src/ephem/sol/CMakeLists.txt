set(OFS_SOL_SYSTEM
    sol/sol.yaml
    mercury/mercury.yaml
    venus/venus.yaml
    earth/earth.yaml
    mars/mars.yaml
    jupiter/jupiter.yaml
    saturn/saturn.yaml
    uranus/uranus.yaml
    neptune/neptune.yaml
)

install(DIRECTORY .
    DESTINATION ${OFS_INSTALL_ROOT_DIR}/systems/sol
    FILES_MATCHING
    PATTERN "*.yaml"
)
