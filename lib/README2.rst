*****************************************
libbract - *brisk replay actions library*
*****************************************

.. contents:: `Table Of Contents`
    :depth: 2

Intro
-----

*libbract* is a static library written in C language that can parse replays
of some strategic games (just WarCraft III for the moment).

Data
----
Currently the library's API yields a following data:

* game's release/patch/build version
* replay's duration
* info on game joiners (name, team number, color, race, ...)
* all messages sent between players
* map info (maximum number of starting positions, map file path) 
* actions per minute of all players

License
-------
The library is licensed under *GPLv2*.

Status
------
The library is still in development and its API is not stable yet.

Supported games
---------------
* WarCraft III: Reign of Chaos, WarCraft III: The Frozen Throne

Dependencies
------------
* zlib

Acknowledgement
---------------
* the library is based on information found in `WarCraft III replay file format description <http://w3g.deepnode.de/>`_