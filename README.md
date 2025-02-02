# qpk11setpassword

### Preface
Based on [investigations & idea](https://github.com/salieff/pk11setpasswordfuncwrapper) of [Alexander Saliev](https://github.com/salieff/).

Can be used to force Chrome, Firefox, Teams and other Chromium based apps to use KDE/GNOME/Unity/XFCE keychain storage for PIN's to PK11 certificates.

Requires Qt5 and [qtkeychain](https://github.com/frankosterfeld/qtkeychain) devel packages installed.

### Build
```
$ mkdir .build
$ cd .build
$ cmake ..
$ cmake --build .
```

### Usgage:

**~/bin/chrome.sh**
```
#!/bin/bash

export LD_PRELOAD="/path/to/libqpk11setpasswordfuncwrapper.so"
exec /usr/bin/google-chrome-stable "$@"

```
