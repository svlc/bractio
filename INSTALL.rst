**********************
*bractio* installation
**********************

0. make sure you have all dependencies satisfied

   * zlib
   * gcc
   * make

1. invoke **make** as a regular user

   * by default the *bractio* and its man page will be installed into
     the ``/usr/local/bin`` and ``/usr/local/share/man/man1``, respectively.
     You can change it by passing the ``prefix`` and/or other variables.
   * you can also pass an extra arguments to a compiler by setting
     the ``OPTCFLAGS`` variable
   * the standard ``DESTDIR`` variable can also be used, especially
     for the testing purposes

2. invoke **make install** as root

   * this will install the *bractio* binary and the *bractio* manual page
   * ``make uninstall`` is also supported

