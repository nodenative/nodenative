{
  'targets': [
    {
      'target_name': 're2',
      'type': 'static_library',
      'include_dirs': [
        '../deps/re2',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '../deps/re2/re2',
        ],
      },
      'sources': [
        '../deps/re2/re2/bitstate.cc',
        '../deps/re2/re2/compile.cc',
        '../deps/re2/re2/dfa.cc',
        '../deps/re2/re2/filtered_re2.cc',
        '../deps/re2/re2/filtered_re2.h',
        '../deps/re2/re2/mimics_pcre.cc',
        '../deps/re2/re2/mimics_pcre.cc',
        '../deps/re2/re2/onepass.cc',
        '../deps/re2/re2/parse.cc',
        '../deps/re2/re2/perl_groups.cc',
        '../deps/re2/re2/prefilter.cc',
        '../deps/re2/re2/prefilter.h',
        '../deps/re2/re2/prefilter_tree.cc',
        '../deps/re2/re2/prefilter_tree.h',
        '../deps/re2/re2/prog.cc',
        '../deps/re2/re2/prog.h',
        '../deps/re2/re2/re2.cc',
        '../deps/re2/re2/re2.h',
        '../deps/re2/re2/regexp.cc',
        '../deps/re2/re2/regexp.h',
        '../deps/re2/re2/set.cc',
        '../deps/re2/re2/set.h',
        '../deps/re2/re2/simplify.cc',
        '../deps/re2/re2/stringpiece.cc',
        '../deps/re2/re2/stringpiece.h',
        '../deps/re2/re2/tostring.cc',
        '../deps/re2/re2/unicode_casefold.cc',
        '../deps/re2/re2/unicode_casefold.h',
        '../deps/re2/re2/unicode_groups.cc',
        '../deps/re2/re2/unicode_groups.h',
        '../deps/re2/re2/variadic_function.h',
        '../deps/re2/re2/walker-inl.h',
        '../deps/re2/util/atomicops.h',
        '../deps/re2/util/flags.h',
        '../deps/re2/util/hash.cc',
        '../deps/re2/util/logging.cc',
        '../deps/re2/util/logging.h',
        '../deps/re2/util/mutex.h',
        '../deps/re2/util/rune.cc',
        '../deps/re2/util/sparse_array.h',
        '../deps/re2/util/sparse_set.h',
        '../deps/re2/util/stringprintf.cc',
        '../deps/re2/util/strutil.cc',
        '../deps/re2/util/utf.h',
        '../deps/re2/util/util.h',
        '../deps/re2/util/valgrind.cc',
        '../deps/re2/util/valgrind.h',
      ],
      'all_dependent_settings' : {
        'cflags':[
            '-std=c++14'
        ]
      },
      'cflags':[
        '-std=c++14'
      ],
      'conditions' : [
        ['OS=="mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS' : ['-std=c++14', '-stdlib=libc++'],
          },
          'link_settings': {
            'libraries': [
              '$(SDKROOT)/System/Library/Frameworks/CoreServices.framework',
              '$(SDKROOT)/System/Library/Frameworks/CoreFoundation.framework'
            ],
          },
          'cflags': [
            '-stdlib=libc++'
          ],
          'all_dependent_settings': {
            'xcode_settings': {
              'OTHER_CPLUSPLUSFLAGS' : ['-std=c++14', '-stdlib=libc++'],
            },
            'link_settings': {
              'libraries': [
                '$(SDKROOT)/System/Library/Frameworks/CoreServices.framework',
                '$(SDKROOT)/System/Library/Frameworks/CoreFoundation.framework'
              ],
            },
            'cflags': [
              '-stdlib=libc++'
            ],
          },
        },
      ]],
    }
  ]
}
