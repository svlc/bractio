*********************************************
librapm, *Replay-Actions-Per-Minute Library*
*********************************************

.. contents:: `Table Of Contents`
    :depth: 2

Intro
-----

*librapm* is a static library written in C language that can inspect replays
of some strategic games (just WarCraft III for the moment)
and obtain all useful information such as:

* game's release version
* replay length
* game joiners
* chat between players
* actions per minute of all players

Replay
++++++
A replay is typically a small (hundreds of kB) binary file
with a proprietary format.

License
-------
The library is licensed under *GPLv2*.

Demonstration
-------------
The library is distributed with a demonstrational program named ``prog``
and a few replays placed in the ``samples/`` directory.

.. code:: bash

  # print length of the replay
  $ ./prog -l ../samples/C-1@Grubby@feRfe.w3g
  +replay_length (ms): 819625

.. code:: bash

  # print information about game's participants
  $ ./prog -j ../samples/C-1@Grubby@feRfe.w3g
  ---->JOINERS
  joiner: EG.Grubby (id: 1, team: 1)
  host: 1, saver: 1, HP: 100, apm 267, leave_time : 00:13:39
  ----------------------------
  joiner: WCG2009_OB_2 (id: 2, team: 12)
  host: 0, saver: 0, HP: 100, apm 0, leave_time : 00:13:38
  ----------------------------
  joiner: WCG2009_OB_1 (id: 3, team: 12)
  host: 0, saver: 0, HP: 100, apm 0, leave_time : 00:13:38
  ----------------------------
  joiner: WCG Referee (id: 4, team: 12)
  host: 0, saver: 0, HP: 100, apm 0, leave_time : 00:13:38
  ----------------------------
  joiner: feRfe` (id: 5, team: 0)
  host: 0, saver: 0, HP: 100, apm 271, leave_time : 00:13:37
  ----------------------------

.. code:: bash

  # print chat
  $./prog -c ../samples/B-1@XtC.Revenant@Huang_feng.w3g
  ---->CHAT MESSAGES
  msg #: 0, from joiner #: 2, at 00:00:48
   gl hf sir
  ----------------------------
  msg #: 1, from joiner #: 1, at 00:00:52
   hf gl sir >D
  ----------------------------
  msg #: 2, from joiner #: 1, at 00:16:34
   gg gratz
  ----------------------------
  msg #: 3, from joiner #: 2, at 00:16:36
   gg
  ----------------------------
  msg #: 4, from joiner #: 2, at 00:16:36
   thx
  ----------------------------

Status
------
The library is still in development and its API is going to change in the future.

Supported games
---------------
* WarCraft III: Reign of Chaos, WarCraft III: The Frozen Throne

Dependencies
------------
* zlib

Acknowledgement
---------------
* the library is based on information found in `WarCraft III replay file format description <http://w3g.deepnode.de/>`_
