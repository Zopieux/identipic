identipic
=========

*identipic* is a graphical tool to create grids of ID pictures to be printed
on photo paper.

The idea is to reduce the price tag of identity pictures, that can cost up to
€1 per photograph when made in dedicated photobooths.


Building
--------

This is a C++14 Qt project, usual build instructions apply::

    $ qmake identipic.pro
    $ make -j4


Usage
-----

#. Run the ``identipic`` binary
#. Click *Load image* (``Ctrl-O``) and pick a face picture
#. Choose the right picture and paper sizes according to your location
#. If needed, adjust brightness and contrast using the dedicated sliders
#. You may play with the other settings, but the defaults should work
#. Click *Build* (``Ctrl-S``) to generate the final image

You may want to adjust the ``DPI`` and *Build* again if the output image is
too small or too big.

You can preview the output image in your default image viewer using ``Ctrl-P``.


Translations
------------

The tool is currently available in:

- English
- French


License
-------

MIT. See ``LICENSE``.

