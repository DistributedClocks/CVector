cmake_minimum_required(VERSION 3.8)
project(CVector)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        example/basic_example.c
        example/client_server.c
        src/mpack/test/mpack-config.h
        src/mpack/test/test-buffer.c
        src/mpack/test/test-buffer.h
        src/mpack/test/test-common.c
        src/mpack/test/test-common.h
        src/mpack/test/test-expect.c
        src/mpack/test/test-expect.h
        src/mpack/test/test-file.c
        src/mpack/test/test-file.h
        src/mpack/test/test-node.c
        src/mpack/test/test-node.h
        src/mpack/test/test-reader.c
        src/mpack/test/test-reader.h
        src/mpack/test/test-system.c
        src/mpack/test/test-system.h
        src/mpack/test/test-write.c
        src/mpack/test/test-write.h
        src/mpack/test/test.c
        src/mpack/test/test.h
        src/mpack/mpack-config.h
        src/mpack/mpack.c
        src/mpack/mpack.h
        src/test/cvec_test.c
        src/vclock/uthash/utarray.h
        src/vclock/uthash/uthash.h
        src/vclock/uthash/utlist.h
        src/vclock/uthash/utringbuffer.h
        src/vclock/uthash/utstring.h
        src/vclock/clock_test.c
        src/vclock/vclock.c
        src/vclock/vclock.h
        src/cvec.c
        src/cvec.h)

add_executable(CVector ${SOURCE_FILES})