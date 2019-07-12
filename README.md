# FlyNote :pencil2:
[![Build Status](https://travis-ci.org/ThiBsc/FlyNote.svg?branch=master)](https://travis-ci.org/ThiBsc/FlyNote)
[![License: MIT](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://opensource.org/licenses/gpl-3.0)

A program to take note on desktop like a Post-It and share on LAN.

![FlyNote](screenshot/sample.jpg)

## Features

* [x] Change the color
* [x] Add/Delete note
* [ ] Reorder the list
* [x] Edit the note title
* [x] Save the list of notes
* [x] Share a note on LAN
* [ ] Send to one user
* [ ] Receive notification

## Compile

```sh
# require qt5-default
mkdir build
cd build
qmake ../flynote.pro
make
```

## Network test

If you want to test the broadcast receiver, start FlyNote, start a shell and write:
```bash
nc -u 192.168.0.255 45454
some
text
```
You will see lines add to the list

## Icons

Note, add, delete, edit and send icon made by [Freepik](https://www.freepik.com/) from [Flaticon](www.flaticon.com)  
broadcast icon made by [smallikeart](https://www.flaticon.com/authors/smalllikeart) from [Flaticon](www.flaticon.com)  
