# Assignment: Forward Error Correction (FEC) using Hamming Coding
## F20 CSCI 466 Networks
### Assigned: 2020-10-28; Due Date: 2020-11-24 (Midnight)

### Project Description 

The network stack employs at least three (3) different error detection and correction (EDC) strategies  explored in class: _Internet checksum_, _parity checking_, and _cyclic redundancy checking_. These three (3) EDC strategies are useful for specific media and _bit error rates_ (BER) over those media, but favor detection over correction, and also provide limited information about where, within the payload, the error occurred, usually resulting in an entire encapsulated unit (within a given layer of the network stack) being retransmitted. If, instead, there were a technique that would (a) be able to detect bit errors with enough certainty so as to correct multiple bit errors within a transmitted encapsulated unit, and (b) in the event there are bit errors that are detected, but unable to be corrected, provide the sending the specificity required to only resent that _part_ of the encapsulated unit, the EDC strategy would allow for considerably higher throughput, especially over media that is subject to high BER.

The EDC strategy described above is known as _forward error correction_ (FEC) and employs two (2) mathematical approaches to work: 

**Hamming Distance**

The Hamming distance between two (2) binary numbers _B<sub>i</sub>_ and _B<sub>j</sub>_ of the same number of bits, written _d(B<sub>i</sub>, B<sub>j</sub>)_ is defined as the number of bit positions that differ. So, _d(1101, 1110) = 2_, whereas _d(1111, 1000) = 3_.

**Code Word Dictionary**

Is a mapping from all the possible combinations of a sequence of _i_ bits, to a larger sequence of _j_ bits, where the _j_ bit _code words_ have minimal overlap and are at least a multiple of _i_ bits in size. An example of a _code word dictionary_ that we will use in this project is as follows:

    |Bit Sequence|Code Word Bit Sequence|
    |------------|----------------------|
    |00|0000 0000|
    |01|1111 1000|
    |10|1000 0111|
    |11|0011 1000|

### How FEC Words 

Say you had an unsigned short _0xA45F_. This would be represented by the binary sequence _1010 0100 0101 1111_. Every pair of binary characters would then be replaced by the corresponding code word from the code word dictionary as follows:

    10 -> 1000 0111
    10 -> 1000 0111

    01 -> 1111 1000
    00 -> 0000 0000
   
    01 -> 1111 1000
    01 -> 1111 1000

    11 -> 0011 1000
    11 -> 0011 1000

This longer, unsigned long is what would actually then be transmitted from the sender to the receiver. 

Let's assume the receiver got the entire ulong (no packet loss), but bit errors occurred with the transmission, such that the following is what was received:

    10010111 10000111 11011100 00001000 11111000 11111000 00111000 10111000

The FEC would attempt to recreate and correct bit errors as follows:

Let's look at the first byte where bit errors have occurred and compute the Hamming distance for this byte against each of the code word bit sequences in the code word dictionary:

    d(1001 0111, 0000 0000) = 5
    d(1001 0111, 1111 1000) = 6
    d(1001 0111, 1000 0111) = 1 -> 10
    d(1001 0111, 0011 1000) = 6

From this, it is clear that the best two-bit bit sequence corresponding to the transmitted code word with the bit errors is _10_. The receiver uses this Hamming distance and selects the correct decoded two-bit bit sequence, correcting for the bit errors.

A similar technique is performed with each of the other code words sent, as follows, thus:

    1001 0111 -> 10
    1000 0111 -> 10

    1101 1100 d(1101 1100, 0000 0000) = 5
              d(1101 1100, 1111 1000) = 3 -> 01
              d(1101 1100, 1000 0111) = 5
              d(1101 1100, 0011 1000) = 4
    0000 1000 d(0000 1000, 0000 0000) = 1 -> 00
              d(0000 1000, 1111 1000) = 4
              d(0000 1000, 1000 0111) = 5
              d(0000 1000, 0011 1000) = 2

    1111 1000 -> 01
    1111 1000 -> 01

    0011 1000 -> 11
    1011 1000 d(1011 1000, 0000 0000) = 4
              d(1011 1000, 1111 1000) = 1 -> 01
              d(1011 1000, 1000 0111) = 6
              d(1011 1000, 0011 1000) = 1 -> 11

The last code word maps to two different possible two-bit bit sequences. This creates ambiguity, with the receiver unable to know how to decode this byte code word. So, the receiver asks the sender to retransmit, but just this one byte, not the entire ulong.

From this exercise, it is clear there are four different possible outcomes for the receiver upon each transmission: 

1. The code word can be directly converted to the decoded two-bit bit sequence
1. The code word contains bit errors, but the Hamming distance selects the correct two-bit bit sequence; correcting the bit errors
1. The code word contains bit errors, but the Hamming distance selects more than a single two-bit bit sequence; requiring the retransmission of just that byte
1. The code word contains so many bit errors within a single byte, that an incorrect two-bit bit sequence is either directly decoded or selected from the Hamming distance; result is to propagate an error to the sender - hopefully caught in an upper network layer.


_Lab Environment_

The course lab and project environment is a Linux virtual machine. The machine name is `csdept10.cs.mtech.edu`. You may access this virtual machine using any available `ssh` (secure shell) client software. One Microsoft Windows-10, you can open a command window and use the built-in `ssh` client software:

Press the `Windows Key` and type `cmd` and then hit `enter`

in the command window, you should then type,

`ssh pcurtiss@csdept10.cs.mtech.edu`

and hit `enter`, where `pcurtiss` is replaced with your username. Your username is first part of your email address, stripped of any trailing numbers. 

You will then be challenged for your password, which you will not see as you type the characters of your password for the sake of privacy. If you cannot remember your password, it may be reset by your instructor. 

Once you have successfully logged in, you will be presented with your Linux prompt. The default prompt from the system will look something similar to the following: 

`pcurtiss@csdept10: ~$`

_Forking Today's Lab Repository_

Open a web browser on your workstation and enter [https://gitlab.cs.mtech.edu](https://gitlab.cs.mtech.edu) into the location field, then login to `GitLab` using the credentials you were selected upon initial account setup.

On the top menu bar of your account page you should see a `Projects` pull-down menu toward the left. `Left-click` on this pull-down menu and select `Your Projects` to see a list of projects to which you have access, as well as your own projects - projects in your own `GitLab` account. One of these projects should look something like the following:

```
Networks / F20 CSCI466 / FEC
```

This project, to which you have access, is in the course (Networks) repository for this term's (F20 CSCI466) offering. In order to be able to modify and work with the files in this repository you must get a copy of this project into your own account on `GitLab`. Making a remote copy from one account on `GitLab` to another account on `GitLab` is called `forking`. 

To fork today's lab repository, `left-click` on the the course project shown above. This will show you a summary of the project, with some of the files shown in a details list, but at the top, to the right of the project name, you should see a few buttons - [A bell icon], [A star icon], and a [Fork] button. `Left-click` on the `fork` button and then select your account as the destination account into which to place the `forked` copy of the project repository - it will take a few minutes for the fork to complete. 

Once completed, you will notice that you are placed in the project contents, but at the top left, the path now looks like,

```
jqpublic / FEC
```

or similar, with our username in the path. This indicates the remote repository has been copied into your account. 

The next step is to add your instructor (and any TAs) as members to this project so they can see the project contents, review the project, and provide a grade for your work. The menu on the left should have a `Members` item. `Left-click` the `Members` menu item on the left and you should be presented with a form. Make sure the `Invite Members` tab at the top is selected and then enter your instructor's (and any TA's) name or email in the `GitLab member or Email address` form field. When you have added the instructor (and any TAs) to this field, `left-click` on the `Choose a role permission` and select `Developer` from the options; you should leave the `Access expiration date` blank. Then `left-click` the `invite` button to add them as members to the project. 

Lastly, `left-click` on the `Project Overview` item in the left menu to display the project information. To the right should be a `blue` button labeled `clone`. `left-click` this button and select the `copy url clipboard` icon to the right of the `ssh` url item to copy the url to the workstation's clipboard. 

_Obtaining Project Files_

Now you are ready to copy the remote project repository for today's lab from your account on the `GitLab` server to your account on `csdept10.cs.mtech.edu`. 

Switch back to the Linux window and type the following: 

```
cd ~/CSCI466/Projects
git clone git@gitlab.cs.mtech.edu:pcurtiss/f20/FEC.git
```

where the specific url is from you pasting the `ssh url` into the window that you copied in your lab step from working with the `GitLab` server - the one shown above is just an example. When you hit `enter` you will be challenged for the passphrase you used when you created your `ssh` key. Enter this passphrase and hit `enter`. Git will now clone (or copy) the remote project repository from your account on `GitLab` to a local repository in your account on `csdept10.cs.mtech.edu` and link the two repositories, such that as you make changes to your local repository, you can `push` these changes back to the remote repository as needed, to update the shared (with your instructor and any TAs) repository on the `GitLab` server. 

Once the `clone` has completed, you should have a new (sub)directory in your `~/CSCI466/Projects` directory named `FEC`, which you can see by using the `ls` command. 

_Completing the Lab Activities_

As you work through the specific activities for this project, you will be asked questions, or to provide information from the environment. This information should be collected in a lab report in a file named `report.pdf` that you should include in the `FEC` directory. You may make this file in Microsoft Word and then save a copy in PDF format and upload (using `scp`) to your project folder. If you are unsure how to do this, ask the instructor or a TA. 

Please perform the following activities to complete this lab assignment: 

1. Write a C# program that will connect to `csdept10.mtech.edu` on TCP port `30120`. Upon connection, you will receive four (4) lines, each terminated in a `\r\n` and each line containing:
    0 0
    1 248
    2 135
    3 56

    These lines represent the `code word dictionary` the sender will use to encode the transmission. The code word dictionary will be followed by a blank line - that is, a `\r\n` by itself.

    After the blank line, you will receive twelve (12) rows of data, each terminated in a `\r\n` sequence as follows:

    0xA45F 0x9787DC08F8F838B8

    where the first hex number is a 2-byte number to be transmitted, and the second hex number is the FEC encoded 8-byte number the receiver gets, with zero or more bit errors due to the transmission.

1. After your receive this information, you may disconnect from the server.

1. Your program is to take the 8-byte number and apply the FEC technique to get back the original 2-byte number. 

1. For each of the twelve (12) numbers received, you are to output:
    1. the original 2-byte number (in hex)
    1. and one of the following:
        1. the number computed from applying FEC
        1. the byte number of any bytes that need to be retransmitted, where byte 0 is LSB and byte 8 is MSB
        1. the message, indicating an error will be propagated to the higher network layers
        
1. Submit your finished project  
    
		git add .  
		git commit -m 'please grade'  
		git push

_Editing Your Lab Project Files_

You may edit your program files either locally, on the Linux system, or remotely, using any number of remote file system schemes. The method selected varies widely depending on your preference. 

To edit locally, on the Linux system, you should use either `nano` or `vim`. Both of these are powerful editors for working with different source code files. To edit a file with nano, you would type, 

```
nano driver.cpp
``` 

for example. You can follow the commands at the bottom of the `nano` window to manipulate the file. I have also included a cheat-sheet for `vim` if you would prefer to use that editor. 

If you are interested in remote editing your files, please see the instructor, a course TA, or a Museum Lab TA for assistance. 