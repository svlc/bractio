********************************************************
*bractio* - program for obtaining data from replay files
********************************************************

.. contents:: `Table Of Contents`
    :depth: 2

Intro
-----

*bractio* is a non-graphical program for inspecting replay files of some videogames.
It can obtain various data such as:

* game's release version
* replay length
* game joiners
* chat between players
* actions per minute of all players

It is the official front-end for the *libbract* library. It is written in C language.

Replay
++++++
A replay is typically a small (hundreds of kB) binary file
with a proprietary format.

License
-------
*bractio* is licensed under *GPLv2*.

Examples
--------

.. code:: bash

  # print length of the replay
  $ bractio -l tft_grubby_vs_ferfe.w3g
  +replay_length (ms): 819625

.. code:: bash

  # print information about game's participants
  $ bractio -j tft_grubby_vs_ferfe.w3g
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
  $bractio -c tft_revenant_vs_huang_feng.w3g
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
The program is still in a development stage.

Supported games
---------------
* WarCraft III: Reign of Chaos, WarCraft III: The Frozen Throne

Dependencies
------------
* zlib
* libbract (static library, part of the project)
