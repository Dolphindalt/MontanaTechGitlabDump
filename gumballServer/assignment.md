# Assignment: Gumball Server in C# (.NET Core)
## F20 CSCI 466 Networks
### Assigned: 2020-09-04; Due Date: 2020-09-14 (Midnight); Reference: Ch2 (Application Protocols) & FSM

### Project Description 

You will create a _server_ program that will take `gumball protocol messages` from the standard input, process these messages according to a _finite state machine_ (FSM), and send resulting messages to the standard output. Later, you will replace standard input and output with a network socket, and you will write a _client_ that will obtain gumballs from your server by having a _conversation_ by exchanging `gumball protocol messages`.

A finite state machine (FSM) is comprised of:
> a set of states _S_ 
>
> a set of inputs &Sigma; - for network protocols, the inputs will become events that are mapped to messages received by the sender
>
> an initial state _S<sub>0</sub>_ 
>
> a set of final states _F_ - for network protocols, there are no final states, just a reset back to _S<sub>0</sub>_ 
>
> a transition function &delta;(s<sub>i</sub> &in; _S_, e &in; &Sigma;) &mapsto; s<sub>j</sub> &in; _S_, where _i_ and _j_ can be the same or different - for network protocols, this function adopts an additional parameter, _f(m)_, a function that (a) sends a message to the sender that generated _e_ &in; &Sigma;, (b) sets the current state to _s<sub>j</sub>_, and (c) optionally enqueues one or more events _e<sup>*</sup>_ &in; &Sigma; into a simple queue _Q_.

Therefore, your Gumball Server must maintain the following: 
> * An initial state for the FSM
> * A current state for the FSM
> * A set of defined states
> * A set of defined events
> * A mapping from input messages to events
> * A simple queue used to process the input - represented as events
> * A data structure to hold the transition table - relating (current state, event) to function(message)

You are provided a simple C# FSM program that uses a `Transition` class derived from the C# `Dictionary` with a custom `CompoundKey` class. These can be used to implement the required FSM for this project. The program also includes a running example - designed to be built via .Net Core. The program example takes from the standard input a string and attempts to accept (recognize) or reject (not recognized) the string as a member of the language `even-a or single-b`. A DFA description is included in the folder `DFAdiagram` for your review. 

### Gumball Protocol

[Initial State] Upon entering this state, the following messages should be sent to the standard output,

> 210 Greeting from the csdept10.cs.mtech.edu Gumball Server (v.2.1) at Fri, 4 Sep 2020 08:22:10 -0600\r\n
>
> 200 INVT Rx<sub>1</sub>:Gx<sub>2</sub>:Yx<sub>3</sub>:Bx<sub>4</sub>\r\n
>
> 210 Gumball Server Ready to dispense gumballs of value _V_ upon receipt of coin\r\n

where x<sub>i</sub>, 1 &le; i &le; 4, represents a positive integer indicating how many of the specific gumball type - R &equiv; Red Gumballs; G &equiv; Green Gumballs; Y &equiv; Yellow Gumballs; B &equiv; Blue Gumballs - remain in the gumball machine. The server will zero a running total that represents the value of the coins collected. The value _v_ is randomly selected such that _V_ % 5 = 0 and 5 &le; _V_ &le; 100.

[COIN Collection] Input message for specifying depositing a coin is follows,

> COIN Qx<sub>1</sub>:Dx<sub>2</sub>:Nx<sub>3</sub>

where x<sub>i</sub>, 1 &le; i &le; 3, represents zero or a positive integer indicating how many of the specific coin denomination - Q &equiv; quater (&le; 4); D &equiv; dime (&le; 10); N &equiv; nickle (&le; 20) - are deposited into the gumball machine. 

The following output message is returned,

> 200 RCVD Qx<sub>1</sub>:Dx<sub>2</sub>:Nx<sub>3</sub>\r\n

where the RCVD Qx<sub>1</sub>:Dx<sub>2</sub>:Nx<sub>3</sub> is an echo back to the client what the server received

If the client issues another COIN message, it will replace the current set of denominations collected by the server. 

If the client issues a, 

> TURN

then the server will process the denominations collected, add their value to a running total, and return the following message, 

> 200  DLTA D [RTRN Qx<sub>1</sub>:Dx<sub>2</sub>:Nx<sub>3</sub>]\r\n

where _D_ is the delta from the cost of a gumball - it can be a positive or negative integer representing how under (negative number) or over (positive) number the sum of collected coins thus far. If _D_ is positive, then the optional, RTRN Qx<sub>1</sub>:Dx<sub>2</sub>:Nx<sub>3</sub> will be provided showing the denominations returned. 

If _D_ &ge; 0, then a gumball is dispensed via the additional message, 

> 200 BALL (R|G|Y|B)\r\n

where only one of R, G, Y, B will follow the BALL message, the running total will be zeroed, and the server will enter its start state. 

[Aborting a Gumball Purchase] At any time, the client may send the message, 

> ABRT

and the server will respond with the message, 

> 200 RTRN Qx<sub>1</sub>:Dx<sub>2</sub>:Nx<sub>3</sub>\r\n

Returning a set of denominations that had, thus far, been collected. The server will then zero its running total and enter its start state. 

[Setting the value of a Gumball]

The client may set the value of a gumball for the server only when the server is in the start state, by issuing the command, 

> BVAL V

where 5 &le; _V_ &le; 100, and _V_ % 5 = 0.

The server will respond with the following message, 

> 200 BVAL:V\r\n

where _V_ is the value received by the server. 

[Error Conditions]

For any input not able to be processed by the server, the following message will be output

> 4xx M\r\n
>
> 410 I\r\n

where 4xx is the status code shown below and _M_ is the corresponding message, and _I_ is the message received on the input by the server. 

|Status Code|Status Message|
|-----------|--------------|
| 400       | Syntax Error |
| 420       | Negative Denomination |
| 421       | Unrecognized Denomination | 
| 422       | Denomination too large|
| 430       | TURN not allowed | 
| 431       | ABRT not allowed | 
| 432       | BVAL not allowed | 
| 440       | BVAL out of range | 
| 441       | BVAL not a multiple of 5 |

---

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
Networks / F20 CSCI466 / gumballServer
```

This project, to which you have access, is in the course (Networks) repository for this term's (F20 CSCI466) offering. In order to be able to modify and work with the files in this repository you must get a copy of this project into your own account on `GitLab`. Making a remote copy from one account on `GitLab` to another account on `GitLab` is called `forking`. 

To fork today's lab repository, `left-click` on the the course project shown above. This will show you a summary of the project, with some of the files shown in a details list, but at the top, to the right of the project name, you should see a few buttons - [A bell icon], [A star icon], and a [Fork] button. `Left-click` on the `fork` button and then select your account as the destination account into which to place the `forked` copy of the project repository - it will take a few minutes for the fork to complete. 

Once completed, you will notice that you are placed in the project contents, but at the top left, the path now looks like,

```
jqpublic / gumballServer 
```

or similar, with our username in the path. This indicates the remote repository has been copied into your account. 

The next step is to add your instructor (and any TAs) as members to this project so they can see the project contents, review the project, and provide a grade for your work. The menu on the left should have a `Members` item. `Left-click` the `Members` menu item on the left and you should be presented with a form. Make sure the `Invite Members` tab at the top is selected and then enter your instructor's (and any TA's) name or email in the `GitLab member or Email address` form field. When you have added the instructor (and any TAs) to this field, `left-click` on the `Choose a role permission` and select `Developer` from the options; you should leave the `Access expiration date` blank. Then `left-click` the `invite` button to add them as members to the project. 

Lastly, `left-click` on the `Project Overview` item in the left menu to display the project information. To the right should be a `blue` button labeled `clone`. `left-click` this button and select the `copy url clipboard` icon to the right of the `ssh` url item to copy the url to the workstation's clipboard. 

_Obtaining Project Files_

Now you are ready to copy the remote project repository for today's lab from your account on the `GitLab` server to your account on `csdept10.cs.mtech.edu`. 

Switch back to the Linux window and type the following: 

```
cd ~/CSCI466/Projects
git clone git@gitlab.cs.mtech.edu:pcurtiss/f20/gumballServer.git
```

where the specific url is from you pasting the `ssh url` into the window that you copied in your lab step from working with the `GitLab` server - the one shown above is just an example. When you hit `enter` you will be challenged for the passphrase you used when you created your `ssh` key. Enter this passphrase and hit `enter`. Git will now clone (or copy) the remote project repository from your account on `GitLab` to a local repository in your account on `csdept10.cs.mtech.edu` and link the two repositories, such that as you make changes to your local repository, you can `push` these changes back to the remote repository as needed, to update the shared (with your instructor and any TAs) repository on the `GitLab` server. 

Once the `clone` has completed, you should have a new (sub)directory in your `~/CSCI466/Projects` directory named `gumballServer`, which you can see by using the `ls` command. 

_Completing the Lab Activities_

As you work through the specific activities for this project, you will be asked questions, or to provide information from the environment. This information should be collected in a lab report in a file named `report.pdf` that you should include in the `gumballServer` directory. You may make this file in Microsoft Word and then save a copy in PDF format and upload (using `scp`) to your project folder. If you are unsure how to do this, ask the instructor or a TA. 

Please perform the following activities to complete this lab assignment: 

1. Review the `gumballServer protocol` above think about designing a Finite State Machine that represents these commands and their actions. For example, 
> COIN can only be received when the server is in either the Initial State or the Coin collecting state
>
> TURN can only be received when the server is in the Coin collecting state
>
> ABRT can be received when in the initial state - and terminates the program
>
> ABRT can be received when in the Coin collecting state, and will return all monies collected, zero out the running total, and reset the server into the initial state
>
> BVAL can only be received when the server is in the Initial State

Make sure your diagram captures all possible input events as well as all error conditions


2. Create a digram of your FSM where each EDGE in the diagram expresses (a) any message send to the client and (b) any events queued. Make sure to represent all aspects of the FSM. Include your diagram in either PNG or PDF format in the project root with a base filename of `GumballServerFSM`.

1. Change to the cloned project directory and create a new console application
> ```
> ch ~/CSCI466/Projects/gumballServer
> dotnet new console
> ```
which should result in a new file `gumballServer.csproj`, `Program.cs`, and an `obj` subdirectory. 

3. Modify the `Program.cs` to implement the required `gumballServer` as specified in the description for this project. Make use of the `FSM` project - located in the subdirectory - as needed, especially the `TransitionTable.cs` and the `CompountKey.cs` files. 

3. You will need to copy these files to the top level of the `gumballServer` project files and then use the `dotnet` utility to add these existing files to your project. [see the man page for dotnet to understand how to use the tool]

4. Once you have coded the `gumballServer` you can build the project by issuing 
> dotnet build

5. You can then run your project by issuing 
> dotnet run 

6. Capture a session of your interaction with the gumballServer where you enter coins that are under the requires amount for gumball dispensing, when you enter coins that are over the amount required, where you change the value of the gumballs, and where you abort an attempt to retrieve a gumball. Finally, include at least one error of every possible status code shown. Include this output in your report. 

7. Submit your finished project
> ```
> git add . 
> git commit -m 'please grade'
> git push
> ```

_Editing Your Lab Project Files_

You may edit your program files either locally, on the Linux system, or remotely, using any number of remote file system schemes. The method selected varies widely depending on your preference. 

To edit locally, on the Linux system, you should use either `nano` or `vim`. Both of these are powerful editors for working with different source code files. To edit a file with nano, you would type, 

```
nano driver.cpp
``` 

for example. You can follow the commands at the bottom of the `nano` window to manipulate the file. I have also included a cheat-sheet for `vim` if you would prefer to use that editor. 

If you are interested in remote editing your files, please see the instructor, a course TA, or a Museum Lab TA for assistance. 