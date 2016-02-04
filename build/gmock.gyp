{
    'targets' : [
        #test
        {
            'target_name' : 'gmock',
            'type' : 'static_library',
            'dependencies': [
                './gtest.gyp:gtest'
            ],
            'include_dirs' : [
                '../deps/googletest/googlemock/include',
                '../deps/googletest/googlemock'
            ],
            'sources' : [
                '../deps/googletest/googlemock/src/gmock-cardinalities.cc',
                '../deps/googletest/googlemock/src/gmock-internal-utils.cc',
                '../deps/googletest/googlemock/src/gmock-matchers.cc',
                '../deps/googletest/googlemock/src/gmock-spec-builders.cc',
                '../deps/googletest/googlemock/src/gmock.cc',
            ],
            'direct_dependent_settings' : {
                'include_dirs' : [
                    '../deps/googletest/googlemock/include'
                ]
            },
            'export_dependent_settings' : [
                './gtest.gyp:gtest'
            ]
        },
        #main
        {
            'target_name' : 'gmock_main',
            'type' : 'static_library',
            'dependencies': [
                'gmock'
            ],
            'sources' : [
                '../deps/googletest/googlemock/src/gmock_main.cc'
            ],
            'direct_dependent_settings' : {
                'include_dirs' : [
                    '../deps/googletest/googletest/include',
                    '../deps/googletest/googlemock/include'
                ]
            }
        }
    ]
}
