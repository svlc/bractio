**********************
*bractio* installation
**********************

1. invoke **make** as a regular user

   * you can pass extra arguments to ``gcc`` by setting the ``OPTCFLAGS`` variable
     for the ``make`` command
   * you can change the default man page path (``/usr/share/man/man1/``)
     by passing the ``MANDIR`` variable
   * the standard ``DESTDIR`` variable can also be used, especially
     for the testing purposes
   * in case of passing your own ``MANDIR`` or ``DESTDIR`` variable(s)
     you must take care of creating destination directories before
     invoking ``make install``

2. invoke **make install** as root

   * this will install the *bractio* binary and the *bractio* manual page
   * ``make uninstall`` is also supported

