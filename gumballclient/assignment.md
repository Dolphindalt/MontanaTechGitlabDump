# Assignment: Gumball Server (network enabled) and Client in C-Language
## F20 CSCI 466 Networks
### Assigned: 2020-09-23; Due Date: 2020-10-08 (Midnight); Reference: Ch2 (Application Protocols) & Gumball Server Project

### Project Description 

You will  network enable your Gumball Server developed in the previous project by leveraging high-level network objects from the C# network libraries. You are provided a working C# server that implements a Trivia Game Proxy - this code can be found in `TriviaGame/TriviaServer` from the main project folder. 

Similarly, you will also write a C-Language client application that communicates with (a) the user to obtain commands from the user and (b) transforms these commands into a Gumball Protocol messages it sends to the Gumball Server, retrieves the response, parses the response and provides this information to the user. 

NOTE - the user is unaware of the Gumball Protocol, so it is insufficient to simply write a client application that collects protocol messages from the user to present to the Gumball Server and present to the user Gumball Protocol messages. You must allow a user friendly set of interactions with the user and transform these into Gumball protocol messages. Similarly, you must interpret Gumball protocol messages from the Gumball Server and present these in a user friendly manner to the end user. 

### Suggestions on how to Proceed

1. You should network enable your Gumball Server code. Firstly, by creating a folder in the main project folder called `gumballServer` in which you move your code from the previous project. Then, by studying the `TriviaGame/TriviaServer` code, adapt the `gumballServer` code to communicate over a TCP (`AF_STREAM`) socket connection.  
Your server should be comprised of the following sections:  
    1. A main service loop that creates a _server socket_ on which to listen (and accept) incoming client connections; remember that the _accept_ socket API call will block until an incoming connection arrives. 
    1. Once a client connection is received, the _accept_ socket API call will return to you a new socket that is connected to the client endpoint (IP address, port). This is the socket your server will use to read and write messages from and to the connected client. 
    1. The server should now create a new thread to handle the specific interactions (protocol message exchanges) with the connected client. 
    1. This new thread should then invoke a _client handler_ that provides the services (through protocol message exchanges) to the connected client. 
    1. The server, in the main thread, should then return to the top of the main loop and wait to accept new incoming client connection. 
1. Once you have the above (1) working, you should use the `nc` command we used in the first - `cowsay` - project to test the network enabled Gumball Server. You could execute,  
		
		% nc csdept10.cs.mtech.edu ${NETPORT}
		210 Greeting from the csdept10.cs.mtech.edu Gumball Server (1.0.0.0) at 9/23/2020 10:33:43 AM
		200 INVT R136:G73:Y19:B25
		210 Gumball Server Ready to dispense gumballs of value 75 upon receipt of coin
		
    should be the output after connecting to the gumball server. You can then issue gumball protocol messages and observe the responses from the server to test the server logic against the gumball protocol FSM and to make sure the messages are exchanged correctly. 
1. Once the testing with the `nc` command works well, you should move on to create the `gumballClient` in the C-Language. You are provided a starter C-Language gumball client that should connect to your gumball server and print out the greeting from the gumball server. You are also provided a complete working `triviaClient` application in the `TriviaGame` subfolder that will print out total number of trivia questions from the trivia server.  
You should use this code as a template for how to use the socket API calls from the C-Language. You will note that such interactions with the socket API from the C-Language relies heavily on data structures (C structs) to represent the uniform representations. However, fundamentally, the same sequence of  
    1. creating a socket - TCP (AF_STREAM)
    1. creating an endpoint (IP address, port)
    1. using the socket to connect to the endpoint
    1. using `recv()` and `send()` to read and write to the connected socket to exchange protocol messages
    1. and then `close()` the socket when communications is no longer needed  

    follows the same steps. This uniform nature fo the socket API  - no matter the language specifics - is at the heart of its success and ease of developing network applications. 

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
Networks / F20 CSCI466 / gumballClient
```

This project, to which you have access, is in the course (Networks) repository for this term's (F20 CSCI466) offering. In order to be able to modify and work with the files in this repository you must get a copy of this project into your own account on `GitLab`. Making a remote copy from one account on `GitLab` to another account on `GitLab` is called `forking`. 

To fork today's lab repository, `left-click` on the the course project shown above. This will show you a summary of the project, with some of the files shown in a details list, but at the top, to the right of the project name, you should see a few buttons - [A bell icon], [A star icon], and a [Fork] button. `Left-click` on the `fork` button and then select your account as the destination account into which to place the `forked` copy of the project repository - it will take a few minutes for the fork to complete. 

Once completed, you will notice that you are placed in the project contents, but at the top left, the path now looks like,

```
jqpublic / gumballClient
```

or similar, with our username in the path. This indicates the remote repository has been copied into your account. 

The next step is to add your instructor (and any TAs) as members to this project so they can see the project contents, review the project, and provide a grade for your work. The menu on the left should have a `Members` item. `Left-click` the `Members` menu item on the left and you should be presented with a form. Make sure the `Invite Members` tab at the top is selected and then enter your instructor's (and any TA's) name or email in the `GitLab member or Email address` form field. When you have added the instructor (and any TAs) to this field, `left-click` on the `Choose a role permission` and select `Developer` from the options; you should leave the `Access expiration date` blank. Then `left-click` the `invite` button to add them as members to the project. 

Lastly, `left-click` on the `Project Overview` item in the left menu to display the project information. To the right should be a `blue` button labeled `clone`. `left-click` this button and select the `copy url clipboard` icon to the right of the `ssh` url item to copy the url to the workstation's clipboard. 

_Obtaining Project Files_

Now you are ready to copy the remote project repository for today's lab from your account on the `GitLab` server to your account on `csdept10.cs.mtech.edu`. 

Switch back to the Linux window and type the following: 

```
cd ~/CSCI466/Projects
git clone git@gitlab.cs.mtech.edu:pcurtiss/f20/gumballClient.git
```

where the specific url is from you pasting the `ssh url` into the window that you copied in your lab step from working with the `GitLab` server - the one shown above is just an example. When you hit `enter` you will be challenged for the passphrase you used when you created your `ssh` key. Enter this passphrase and hit `enter`. Git will now clone (or copy) the remote project repository from your account on `GitLab` to a local repository in your account on `csdept10.cs.mtech.edu` and link the two repositories, such that as you make changes to your local repository, you can `push` these changes back to the remote repository as needed, to update the shared (with your instructor and any TAs) repository on the `GitLab` server. 

Once the `clone` has completed, you should have a new (sub)directory in your `~/CSCI466/Projects` directory named `gumballClient`, which you can see by using the `ls` command. 

_Completing the Lab Activities_

As you work through the specific activities for this project, you will be asked questions, or to provide information from the environment. This information should be collected in a lab report in a file named `report.pdf` that you should include in the `gumballServer` directory. You may make this file in Microsoft Word and then save a copy in PDF format and upload (using `scp`) to your project folder. If you are unsure how to do this, ask the instructor or a TA. 

Please perform the following activities to complete this lab assignment: 

1. Review the provided code - both the C# and the C-Language code and study this code ! The more you understand how this code works, the easier it will be to adapt your own code - from the `gumballServer` project - to create a network enabled version of the project. 

1. Copy your `gumballServer` code from your previous project to a subfolder of this project named `gumballServer` and adapt the code to manage incoming client connections and exchange gumball protocol messages through the connected client socket. 

1. Once completed, test your code thoroughly using the `nc` command before proceeding to write your `gumballClient` application. You must exercise all gumball protocol commands using the `nc` command and capture the output to place in the accompanying report. 

1. Once your gumball server is working well and tested, you can move on to write a gumball client application that connects to the gumball server and exchanges gumball protocol messages to purchase gumballs. You should adapt - or make use of - the code provided in the `gumballClient.c` code provided. 

1. Your client program must take into account a user that is **unaware** of the gumball protocol and provide a useful interface for providing monies to the gumball server and extracting a gumball. Your client must manage all protocol message exchanges with your gumball server to effect the end-user's transactions. 

1. Once written, you must demonstrate an end-user  
    1. obtaining a gumball from the gumball server by providing exact change to the server
    1. obtaining a gumball from the gumball server by providing too much change to the server
    1. aborting the purchase of a gumball after providing some amount of change to the server
    1. ending an interaction with the gumball server  
    
    All of these interactions must be captured in the report file to be submitted with your code. 

1. A Makefile has been provided with your `gumballClient.c` code and you should feel free to use this Makefile - possibly modified - to build your client applicaiton. 

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