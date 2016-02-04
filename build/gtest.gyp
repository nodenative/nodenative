{
    'targets' : [
        #test
        {
            'target_name' : 'gtest',
            'type' : 'static_library',
            'include_dirs' : [
                '../deps/googletest/googletest/include',
                '../deps/googletest/googletest'
            ],
            'sources' : [
                '../deps/googletest/googletest/src/gtest.cc',
                '../deps/googletest/googletest/src/gtest-death-test.cc',
                '../deps/googletest/googletest/src/gtest-filepath.cc',
                '../deps/googletest/googletest/src/gtest-port.cc',
                '../deps/googletest/googletest/src/gtest-printers.cc',
                '../deps/googletest/googletest/src/gtest-test-part.cc',
                '../deps/googletest/googletest/src/gtest-typed-test.cc',
            ],
            'direct_dependent_settings' : {
                'include_dirs' : [
                    '../deps/googletest/googletest/include'
                ]
            }
        },
        #main
        {
            'target_name' : 'gtest_main',
            'type' : 'static_library',
            'dependencies': [
                'gtest'
            ],
            'sources' : [
                '../deps/googletest/googletest/src/gtest_main.cc'
            ],
            'direct_dependent_settings' : {
                'include_dirs' : [
                    '../deps/googletest/googletest/include'
                ]
            }
        }
    ]
}
