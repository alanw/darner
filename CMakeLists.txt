PROJECT(darner)

SET(DARNER_VERSION 0.2.0)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

CMAKE_POLICY(SET CMP0005 NEW) # don't complain about escaping the double-quotes below around DARNER_VERSION

ADD_DEFINITIONS(-Wall -Wno-return-type -O2 -g -DNDEBUG -DDARNER_VERSION="${DARNER_VERSION}")

FIND_PACKAGE(Boost COMPONENTS thread system program_options unit_test_framework filesystem)

find_library(LIBLEVELDB_LIBRARY NAMES leveldb)
find_library(LIBSNAPPY_LIBRARY NAMES snappy)

INCLUDE_DIRECTORIES(include
                    ${Boost_INCLUDE_DIRS}
                    )

ADD_EXECUTABLE(darner
               src/net/handler
               src/net/request
               src/util/log
               src/queue/iqstream
               src/queue/oqstream
               src/queue/queue
               src/main
               )

ADD_EXECUTABLE(db
               bench/db
               )

ADD_EXECUTABLE(test_darner
               src/net/request
               src/queue/iqstream
               src/queue/oqstream
               src/queue/queue
               src/util/log
               tests/queue
               tests/request
               tests/main
               )

TARGET_LINK_LIBRARIES(darner pthread ${Boost_LIBRARIES} ${LIBLEVELDB_LIBRARY} ${LIBSNAPPY_LIBRARY})
TARGET_LINK_LIBRARIES(db ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(test_darner pthread ${Boost_LIBRARIES} ${LIBLEVELDB_LIBRARY} ${LIBSNAPPY_LIBRARY})

INSTALL(TARGETS darner DESTINATION bin)
