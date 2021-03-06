TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

SOURCES += \
    test/smoke_test.cpp \
    test/policies_test.cpp \
    test/pool_test.cpp \
    test/statistic_policy_test.cpp \
    test/stl_test.cpp \
    test/linear_alloc_test.cpp \
    test/details/alloc_type_traits_test.cpp \
    test/details/policies_list_test.cpp \
    test/details/rebind_test.cpp \
    test/concepts/concepts_test.cpp

HEADERS += \
    include/allocator/allocator.hpp \
    include/allocator/alloc_policies.hpp \
    include/allocator/alloc_traits.hpp \
    include/allocator/macro.hpp \
    include/allocator/none_policy.hpp \
    include/allocator/pointer_cast.hpp \
    include/allocator/pool_allocation.hpp \
    include/allocator/statistic_policy.hpp \
    include/allocator/details/memory_pool.hpp \
    include/allocator/details/policies_list.hpp \
    include/allocator/alloc_type_traits.hpp \
    include/allocator/details/rebind.hpp \
    include/allocator/linear_allocation.hpp \
    include/allocator/details/linear_storage.hpp \
    include/allocator/pointer_concepts.hpp \
    include/allocator/details/alloc_type_traits.hpp \
    include/allocator/details/is_swappable.hpp \
    include/allocator/alloc_policy_concept.hpp \
    include/allocator/concepts/alloc_policy_concept.hpp \
    include/allocator/concepts/pointer_concepts.hpp \
    test/test_types.hpp

INCLUDEPATH += include/allocator test

LIBS += -lgtest -lgtest_main -pthread

