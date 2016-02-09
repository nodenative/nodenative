{
    'targets' : [
        #server
        {
            'target_name' : 'webserver',
            'type' : 'executable',
            'dependencies': [
                './native.gyp:node_native'
            ],
            'sources' : [
                '../samples/webserver.cpp'
            ]
        },
        #client
        {
            'target_name' : 'webclient',
            'type' : 'executable',
            'dependencies': [
                './native.gyp:node_native'
            ],
            'sources' : [
                '../samples/webclient.cpp'
            ]
        },
        #test
        {
            'target_name' : 'test',
            'type' : 'executable',
            'dependencies': [
                './native.gyp:node_native',
                './gmock.gyp:gmock_main'
            ],
            'sources' : [
                '../test/basic_test.cpp',
                '../test/crypto_test.cpp',
                '../test/http_test.cpp',
                '../test/timer_test.cpp',
                '../test/async_test.cpp',
                '../test/future_then_test.cpp',
                '../test/future_error_test.cpp',
                '../test/worker_test.cpp',
            ]
        }
    ]
}
