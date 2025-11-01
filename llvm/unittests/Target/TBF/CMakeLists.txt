include_directories(
        ${LLVM_MAIN_SRC_DIR}/lib/Target/TBF
        ${LLVM_BINARY_DIR}/lib/Target/TBF
)

set(LLVM_LINK_COMPONENTS
        TBFCodeGen
        TBFDesc
        TBFInfo
        Analysis
        CodeGen
        Core
        MC
        SelectionDAG
        TargetParser
)

add_llvm_target_unittest(TBFTests
        SBFInstrInfoTest.cpp
)
