{
  'targets' : [
        #native
        {
            'target_name' : 'node_native',
            'type' : 'static_library',
            'nnative_use_openssl%': 'true',
            'nnative_shared_openssl%': 'false',
            'nnative_target_type%': 'static_library',
            'variables': {
              'nnative_regex_name%': 're2', # possible values: re2, std, boost
            },
            'dependencies': [
                '../deps/libuv/uv.gyp:libuv',
                '../deps/http-parser/http_parser.gyp:http_parser',
            ],
            'include_dirs' : [
                '../deps/libuv/include',
                '../deps/http-parser',
                '../include'
            ],
            'sources' : [
                '../src/async/AsyncBase.cpp',
                '../src/async/ActionCallback.cpp',
                '../src/async/FutureShared.cpp',
                '../src/async/FutureSharedResolver.cpp',
                '../src/base/Handle.cpp',
                '../src/base/Stream.cpp',
                '../src/crypto/utils.cpp',
                '../src/crypto/PBKDF2.cpp',
                '../src/crypto.cpp',
                '../src/fs.cpp',
                '../src/http/ClientRequest.cpp',
                '../src/http/ClientResponse.cpp',
                '../src/http/HttpUtils.cpp',
                '../src/http/MessageBase.cpp',
                '../src/http/IncomingMessage.cpp',
                '../src/http/OutgoingMessage.cpp',
                '../src/http/Transaction.cpp',
                '../src/http/Server.cpp',
                '../src/http/ServerPlugin.cpp',
                '../src/http/ServerRequest.cpp',
                '../src/http/ServerResponse.cpp',
                '../src/http/UrlObject.cpp',
                '../src/http.cpp',
                '../src/Loop.cpp',
                '../src/net/net_utils.cpp',
                '../src/net/Tcp.cpp',
                '../src/Regex.cpp',
                '../src/Timer.cpp',
                '../src/worker/WorkerBase.cpp',
                '../src/worker/WorkerCallback.cpp',
                '../src/UriTemplate.cpp',
                '../src/UriTemplateFormat.cpp',
                '../src/UriTemplateValue.cpp',
            ],
            'direct_dependent_settings' : {
                'include_dirs' : [
                    '../include',
                    '../deps/libuv/include',
                    '../deps/http-parser'
                ]
            },
            'all_dependent_settings' : {
                'cflags':[
                    '-std=c++14'
                ]
            },
            'cflags':[
                '-std=c++14'
            ],
            'conditions' : [
                ['nnative_regex_name=="re2"', {
                  'dependencies': [
                    're2.gyp:re2',
                  ],
                  'defines': [
                    'NNATIVE_USE_RE2=1',
                    'NNATIVE_USE_STDREGEX=0',
                    'NNATIVE_USE_BOOSTREGEX=0',
                  ],
                  'all_dependent_settings': {
                    'defines': [
                      'NNATIVE_USE_RE2=1',
                      'NNATIVE_USE_STDREGEX=0',
                      'NNATIVE_USE_BOOSTREGEX=0',
                    ],
                  }
                }, 'nnative_regex_name=="std"', {
                  'defines': [
                    'NNATIVE_USE_RE2=0',
                    'NNATIVE_USE_STDREGEX=1',
                    'NNATIVE_USE_BOOSTREGEX=0',
                  ],
                  'all_dependent_settings': {
                    'defines': [
                      'NNATIVE_USE_RE2=0',
                      'NNATIVE_USE_STDREGEX=1',
                      'NNATIVE_USE_BOOSTREGEX=0',
                    ],
                  }
                }, 'nnative_regex_name=="boost"', {
                  'defines': [
                    'NNATIVE_USE_RE2=0',
                    'NNATIVE_USE_STDREGEX=0',
                    'NNATIVE_USE_BOOSTREGEX=1',
                  ],
                  'ldflags': [
                    '-lboost_regex',
                    '-lboost_iostreams'
                  ],
                  'all_dependent_settings': {
                    'defines': [
                      'NNATIVE_USE_RE2=0',
                      'NNATIVE_USE_STDREGEX=0',
                      'NNATIVE_USE_BOOSTREGEX=1',
                    ],
                    'ldflags': [
                      '-lboost_regex',
                      '-lboost_iostreams'
                    ],
                  },
                }, {
                  'defines': [
                    'NNATIVE_USE_RE2=0',
                    'NNATIVE_USE_STDREGEX=0',
                    'NNATIVE_USE_BOOSTREGEX=0',
                  ]
                }
                ],
                ['OS=="mac"', {
                    'xcode_settings': {
                        'OTHER_CPLUSPLUSFLAGS' : ['-std=c++14', '-stdlib=libc++'],
                        #'MACOSX_DEPLOYMENT_TARGET': '10.7',
                        #'OTHER_LDFLAGS': ['-stdlib=libc++']
                        #'ARCHS': '$(ARCHS_STANDARD_64_BIT)'
                    },
                    'link_settings': {
                        'libraries': [
                            '$(SDKROOT)/System/Library/Frameworks/CoreServices.framework',
                            '$(SDKROOT)/System/Library/Frameworks/CoreFoundation.framework'
                        ]
                    },
                    'cflags': [
                        '-stdlib=libc++'
                    ],
                    'all_dependent_settings': {
                        'xcode_settings': {
                            'OTHER_CPLUSPLUSFLAGS' : ['-std=c++14', '-stdlib=libc++'],
                            #'MACOSX_DEPLOYMENT_TARGET': '10.7',
                            #'OTHER_LDFLAGS': ['-stdlib=libc++']
                            #'ARCHS': '$(ARCHS_STANDARD_64_BIT)'
                        },
                        'link_settings': {
                            'libraries': [
                                '$(SDKROOT)/System/Library/Frameworks/CoreServices.framework',
                                '$(SDKROOT)/System/Library/Frameworks/CoreFoundation.framework'
                            ]
                        },
                        'cflags': [
                            '-stdlib=libc++'
                        ]
                    }
                }],
                ['OS=="linux"', {
                    'defines': [ '_GNU_SOURCE' ],
                }],
                ['nnative_use_openssl=="true"', {
                    'defines': [ 'HAVE_OPENSSL=1' ],
                    'sources': [
                    ],
                    'conditions': [
                        [ 'nnative_shared_openssl=="false"', {
                            'dependencies': [
                                '../deps/openssl/openssl.gyp:openssl',

                                # for tests
                                '../deps/openssl/openssl.gyp:openssl-cli',
                            ],
                            # Do not let unused OpenSSL symbols to slip away
                            'conditions': [
                                # -force_load or --whole-archive are not applicable for
                                # the static library
                                [ 'nnative_target_type!="static_library"', {
                                    'xcode_settings': {
                                        'OTHER_LDFLAGS': [
                                            '-Wl,-force_load,<(PRODUCT_DIR)/<(OPENSSL_PRODUCT)',
                                        ]
                                    },
                                    'conditions': [
                                        ['OS in "linux freebsd"', {
                                            'ldflags': [
                                                '-Wl,--whole-archive <(PRODUCT_DIR)/<(OPENSSL_PRODUCT)',
                                                '-Wl,--no-whole-archive',
                                            ]
                                        }]
                                    ]
                                }]
                            ]
                        }]
                    ]
                },
                {
                    'defines': [ 'HAVE_OPENSSL=0' ]
                }]
            ]
        }
    ]
}
