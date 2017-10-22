{
  "targets": [
        {
            "target_name": "hackrf",
            "sources": [
                "src/bindings.cc",
                "src/device.cc"
            ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")"
            ],
            "dependencies": [ "libhackrf"  ],
            "conditions": [
                ['OS=="mac"', {
                    'xcode_settings': {
                        'OTHER_LDFLAGS': [ '-framework', 'CoreFoundation', '-framework', 'IOKit' ],
                        'SDKROOT': 'macosx',
                        'MACOSX_DEPLOYMENT_TARGET': '10.7',
                    },
                }]
            ]
        },
        {
            "target_name": "libhackrf",
            "type": "static_library",
            "sources": [ "src/hackrf.c" ],
            "include_dirs": [ "deps/libusb/libusb" ],
            "dependencies": [ "deps/libusb.gypi:libusb" ]
        }
    ]
}
