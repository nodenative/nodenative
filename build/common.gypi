{
  'variables': {
    'visibility%': 'hidden',         # V8's visibility setting
    'target_arch%': 'ia32',          # set v8's target architecture
    'host_arch%': 'ia32',            # set v8's host architecture
    'library%': 'static_library',    # allow override to 'shared_library' for DLL/.so builds
    'uv_library': 'static_library',  # allow override to 'shared_library' for DLL/.so builds
    'native_library': 'static_library',
    'component%': 'static_library',  # NB. these names match with what V8 expects
    'msvs_multi_core_compile': '0',  # we do enable multicore compiles, but not using the V8 way
    'gcc_version%': 'unknown',
    'clang%': 0,
    # Enable building with ASAN (Clang's -fsanitize=address option).
    # -fsanitize=address only works with clang, but asan=1 implies clang=1
    'asan%': 0,
    # Enable coverage gathering instrumentation in sanitizer tools. This flag
    # also controls coverage granularity (1 for function-level coverage, 2
    # for block-level coverage).
    'sanitizer_coverage%': 0,

    # Enable building with LSan (Clang's -fsanitize=leak option).
    # -fsanitize=leak only works with clang, but lsan=1 implies clang=1
    'lsan%': 0,

    # Enable building with TSan (Clang's -fsanitize=thread option).
    # -fsanitize=thread only works with clang, but tsan=1 implies clang=1
    'tsan%': 0,
    'tsan_blacklist%': './tsan/ignores.txt',

    # Enable building with MSan (Clang's -fsanitize=memory option).
    # MemorySanitizer only works with clang, but msan=1 implies clang=1
    'msan%': 0,
    'msan_blacklist%': './msan/blacklist.txt',
    # Track where uninitialized memory originates from. From fastest to
    # slowest: 0 - no tracking, 1 - track only the initial allocation site, 2
    # - track the chain of stores leading from allocation site to use site.
    'msan_track_origins%': 2,
  },

  'target_defaults': {
    'default_configuration': 'Debug',
    'configurations': {
      'Debug': {
        'defines': [ 'DEBUG', '_DEBUG' ],
        'cflags': [
            '-g',
            '-O0',
            '-fwrapv'#,
            #'-std=c++11'
        ],
        'msvs_settings': {
          'VCCLCompilerTool': {
            'target_conditions': [
              ['library=="static_library"', {
                'RuntimeLibrary': 1, # static debug
              }, {
                'RuntimeLibrary': 3, # DLL debug
              }],
            ],
            'Optimization': 0, # /Od, no optimization
            'MinimalRebuild': 'false',
            'OmitFramePointers': 'false',
            'BasicRuntimeChecks': 3, # /RTC1
          },
          'VCLinkerTool': {
            'LinkIncremental': 2, # enable incremental linking
          },
        },
        'xcode_settings': {
          'GCC_OPTIMIZATION_LEVEL': '0',
          'OTHER_CFLAGS': [ '-Wno-strict-aliasing' ],
        },
        'conditions': [
          ['OS != "win"', {
            'defines': [ 'EV_VERIFY=2' ],
          }],
        ]
      },
      'Release': {
        'defines': [ 'NDEBUG' ],
        'cflags': [
          '-O3',
          '-fstrict-aliasing',
          '-fomit-frame-pointer',
          '-fdata-sections',
          '-ffunction-sections'#,
          #'-std=c++11'
        ],
        'msvs_settings': {
          'VCCLCompilerTool': {
            'target_conditions': [
              ['library=="static_library"', {
                'RuntimeLibrary': 0, # static release
              }, {
                'RuntimeLibrary': 2, # debug release
              }],
            ],
            'Optimization': 3, # /Ox, full optimization
            'FavorSizeOrSpeed': 1, # /Ot, favour speed over size
            'InlineFunctionExpansion': 2, # /Ob2, inline anything eligible
            'WholeProgramOptimization': 'true', # /GL, whole program optimization, needed for LTCG
            'OmitFramePointers': 'true',
            'EnableFunctionLevelLinking': 'true',
            'EnableIntrinsicFunctions': 'true',
          },
          'VCLibrarianTool': {
            'AdditionalOptions': [
              '/LTCG', # link time code generation
            ],
          },
          'VCLinkerTool': {
            'LinkTimeCodeGeneration': 1, # link-time code generation
            'OptimizeReferences': 2, # /OPT:REF
            'EnableCOMDATFolding': 2, # /OPT:ICF
            'LinkIncremental': 1, # disable incremental linking
          },
        },
      }
    },
    'msvs_settings': {
      'VCCLCompilerTool': {
        'StringPooling': 'true', # pool string literals
        'DebugInformationFormat': 3, # Generate a PDB
        'WarningLevel': 3,
        'BufferSecurityCheck': 'true',
        'ExceptionHandling': 1, # /EHsc
        'SuppressStartupBanner': 'true',
        'WarnAsError': 'false',
        'AdditionalOptions': [
           '/MP', # compile across multiple CPUs
         ],
      },
      'VCLibrarianTool': {
      },
      'VCLinkerTool': {
        'GenerateDebugInformation': 'true',
        'RandomizedBaseAddress': 2, # enable ASLR
        'DataExecutionPrevention': 2, # enable DEP
        'AllowIsolation': 'true',
        'SuppressStartupBanner': 'true',
        'target_conditions': [
          ['_type=="executable"', {
            'SubSystem': 1, # console executable
          }],
        ],
      },
    },
    'conditions': [
      ['OS == "win"', {
        'msvs_cygwin_shell': 0, # prevent actions from trying to use cygwin
        'defines': [
          'WIN32',
          # we don't really want VC++ warning us about
          # how dangerous C functions are...
          '_CRT_SECURE_NO_DEPRECATE',
          # ... or that C implementations shouldn't use
          # POSIX names
          '_CRT_NONSTDC_NO_DEPRECATE',
        ],
        'target_conditions': [
          ['target_arch=="x64"', {
            'msvs_configuration_platform': 'x64'
          }]
        ]
      }],
      ['OS in "freebsd linux openbsd solaris android"', {
        'cflags': [ '-Wall' ],
#TODO: find a solution to use '-fno-rtti'
#        'cflags_cc': [ '-fno-rtti', '-fno-exceptions' ],
        'target_conditions': [
          ['_type=="static_library"', {
            'standalone_static_library': 1, # disable thin archive which needs binutils >= 2.19
          }],
        ],
        'conditions': [
          [ 'host_arch != target_arch and target_arch=="ia32"', {
            'cflags': [ '-m32' ],
            'ldflags': [ '-m32' ],
          }],
          [ 'OS=="linux"', {
            'cflags': [ '-ansi' ],
          }],
          [ 'OS=="solaris"', {
            'cflags': [ '-pthreads' ],
            'ldflags': [ '-pthreads' ],
          }],
          [ 'OS not in "solaris android"', {
            'cflags': [ '-pthread' ],
            'ldflags': [ '-pthread' ],
          }],
          [ 'visibility=="hidden" and (clang==1 or gcc_version >= 40)', {
            'cflags': [ '-fvisibility=hidden' ],
          }],
        ],
      }],
      ['OS=="mac"', {
        'xcode_settings': {
          'ALWAYS_SEARCH_USER_PATHS': 'NO',
          'GCC_CW_ASM_SYNTAX': 'NO',                # No -fasm-blocks
          'GCC_DYNAMIC_NO_PIC': 'NO',               # No -mdynamic-no-pic
                                                    # (Equivalent to -fPIC)
          'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',        # -fno-exceptions
          'GCC_ENABLE_CPP_RTTI': 'YES',              # -fno-rtti
          'GCC_ENABLE_PASCAL_STRINGS': 'NO',        # No -mpascal-strings
          # GCC_INLINES_ARE_PRIVATE_EXTERN maps to -fvisibility-inlines-hidden
          'GCC_INLINES_ARE_PRIVATE_EXTERN': 'YES',
          'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',      # -fvisibility=hidden
          'GCC_THREADSAFE_STATICS': 'NO',           # -fno-threadsafe-statics
          'PREBINDING': 'NO',                       # No -Wl,-prebind
          'USE_HEADERMAP': 'NO',
          'OTHER_CFLAGS': [
            '-fstrict-aliasing',
          ],
          'WARNING_CFLAGS': [
            '-Wall',
            '-Wendif-labels',
            '-W',
            '-Wno-unused-parameter',
          ],
        },
        'conditions': [
          ['target_arch=="ia32"', {
            'xcode_settings': {'ARCHS': ['i386']},
          }],
          ['target_arch=="x64"', {
            'xcode_settings': {'ARCHS': ['x86_64']},
          }],
        ],
        'target_conditions': [
          ['_type!="static_library"', {
            'xcode_settings': {'OTHER_LDFLAGS': ['-Wl,-search_paths_first']},
          }],
        ],
      }],
     ['OS=="solaris"', {
       'cflags': [ '-fno-omit-frame-pointer' ],
       # pull in V8's postmortem metadata
       'ldflags': [ '-Wl,-z,allextract' ]
     }],
     ['(clang!=0 or OS !="win" ) and (asan==1 or lsan==1 or tsan==1 or msan==1)', {
       'cflags': [
         '-fno-omit-frame-pointer',
         #'-gline-tables-only',
         '-g',
       ],
       'cflags!': [
         '-fomit-frame-pointer',
       ],
       'ldflags!': [
         # Functions interposed by the sanitizers can make ld think
         # that some libraries aren't needed when they actually are,
         '-Wl,--as-needed',
       ],
       'defines': [
         'MEMORY_TOOL_REPLACES_ALLOCATOR',
         'MEMORY_SANITIZER_INITIAL_SIZE',
       ],
     }],
     ['clang!=0 and OS == "mac" and (asan==1 or lsan==1 or tsan==1 or msan==1)', {
       'xcode_settings': {
         'OTHER_CFLAGS': [
           '-fno-omit-frame-pointer',
           '-gline-tables-only',
         ],
         'OTHER_CFLAGS!': [
           '-fomit-frame-pointer',
         ],
       }
     }],
     ['asan==1 and (clang!=0 or OS !="win")', {
       'cflags': [
         '-fsanitize=address',
       ],
       'ldflags': [
         '-fsanitize=address',
         '-lasan',
       ]
     }],
     ['asan==1 and OS=="mac" and clang!=0', {
       'xcode_settings': {
         'OTHER_CFLAGS': [
           '-g -fsanitize=address',
           #'-mllvm -asan-globals=0',
           '-gline-tables-only',
         ],
       },
       'target_conditions': [
         ['_type!="static_library"', {
           'xcode_settings': {
             'OTHER_LDFLAGS': [
               '-g -fsanitize=address',
             ],
           }
         }],
       ],
     }],
     ['sanitizer_coverage!=0 and clang!=0', {
       'target_conditions': [
         ['_toolset=="target"', {
           'cflags': [
             '-fsanitize-coverage=<(sanitizer_coverage)',
           ],
         }],
       ],
       'defines': [
         'SANITIZER_COVERAGE',
       ]
     }],
     ['lsan==1 and (clang!=0 or OS !="win")', {
       'cflags': [
         '-fsanitize=leak',
       ],
       'ldflags': [
         '-fsanitize=leak',
       ],
       'defines': [
         'LEAK_SANITIZER',
         'WTF_USE_LEAK_SANITIZER=1',
       ]
     }],
     ['tsan==1 and clang!=0', {
       'target_conditions': [
         ['_toolset=="target"', {
           'cflags': [
             '-fsanitize=thread',
             '-fsanitize-blacklist=<(tsan_blacklist)',
           ],
           'ldflags': [
             '-fsanitize=thread',
           ],
           'defines': [
             'THREAD_SANITIZER',
             'DYNAMIC_ANNOTATIONS_EXTERNAL_IMPL=1',
             'WTF_USE_DYNAMIC_ANNOTATIONS_NOIMPL=1',
           ],
         }],
       ],
     }],
     ['msan==1 and clang!=0', {
       'cflags': [
         '-fsanitize=memory',
         '-fsanitize-memory-track-origins=<(msan_track_origins)',
         '-fsanitize-blacklist=<(msan_blacklist)',
       ],
       'ldflags': [
         '-fsanitize=memory',
       ],
       'defines': [
         'THREAD_SANITIZER',
         'DYNAMIC_ANNOTATIONS_EXTERNAL_IMPL=1',
         'WTF_USE_DYNAMIC_ANNOTATIONS_NOIMPL=1',
       ],
     }],
    ],
  },
}
