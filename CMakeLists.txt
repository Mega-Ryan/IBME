cmake_minimum_required(VERSION 3.16)

project(IB-ME)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCESOP
    src/DiscreteUniformSampler.cpp
    src/DiscreteGaussianSampler.cpp
    src/Hash.cpp
    src/Tree.cpp
    src/Matrix.cpp
    src/MP12.cpp
    src/IB-ME.cpp
    src/benchmarkOp.cpp
    )

add_executable(benchmarkOp ${SOURCESOP})

set(SOURCESFUNC
    src/DiscreteUniformSampler.cpp
    src/DiscreteGaussianSampler.cpp
    src/Hash.cpp
    src/Tree.cpp
    src/Matrix.cpp
    src/MP12.cpp
    src/IB-ME.cpp
    src/benchmarkIBMEfunc.cpp
    )

add_executable(benchmarkIBMEfunc ${SOURCESFUNC})

set(SOURCESIBME
    src/DiscreteUniformSampler.cpp
    src/DiscreteGaussianSampler.cpp
    src/Hash.cpp
    src/Tree.cpp
    src/Matrix.cpp
    src/MP12.cpp
    src/IB-ME.cpp
    src/benchmarkIBME.cpp
    )
add_executable(benchmarkIBME ${SOURCESIBME})

