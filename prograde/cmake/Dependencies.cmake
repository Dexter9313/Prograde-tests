find_package(Boost REQUIRED)
find_package(Qt5 COMPONENTS Concurrent REQUIRED)

set(PROJECT_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${Qt5Concurrent_INCLUDE_DIRS})
set(PROJECT_LIBRARIES ${Boost_LIBRARIES} -lquadmath Qt5::Concurrent)

