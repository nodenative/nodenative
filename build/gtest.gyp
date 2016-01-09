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
                '../deps/googletest/googletest/src/gtest-all.cc'
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
