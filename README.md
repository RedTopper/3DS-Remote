# 3DS-Remote
Sends and receives IR and stuff!

###IN ORDER TO USE IR YOU MUST HAVE ONE OF THE FOLLOWING GAMES:
- Cubic Ninja(US)
- Ocarina of Time 3D
- Toki no Ocarina 3D (OoT 3D)
- Dead or Alive Dimensions

Please make sure you have 3DSRemote.xml in the same directory as CPPTest.3dsx. In order to select a target, use the D pad (left and right). Report findings at https://gbatemp.net/threads/infra-red-resource-thread.399336/.

### How to use
- Press start to change between record and send mode
- Press UP or DOWN on the DPad to change the bitrate of the 3DS. See http://3dbrew.org/wiki/IRU:SetBitRate for details. 
- Press A to record or send data (Code has the ability to expand to recording to all buttons, but that takes time. Just use A to record if recording and to send if sending). 

### Notes
- Sometimes the 3DS does not pick up IR. Try to re-record the IR again and see if the bottom screen changes.
- Sometimes IR does not exit. Press and hold the power button to fix (rare).

### FAQ:
- I got IR Init Error: d8e06406! How do I fix??
 - Did you launch the homebrew with one of the mentioned games?
- There is a bug. What do?
 - Report it on GitHub. I'll see what I can do.
- When I compile it, it breaks everything.
 - I am using a modified version of smeas library. I'll see if I can upload it along with this (you will need to recompile his library with my changes)
- Why does IR Transfer error show ffffffff?
 - Have you tried transferring something?
- It doesn't work on my TV
 - I don't think it works on any TVs yet. It's more of a Proof of Concept. 
