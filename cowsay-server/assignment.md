# Assignment: Environment Setup and Linux Based Server
## F20 CSCI 466 Networks
### Assigned: 2020-08-19; Due Date: 2020-08-27 (Midnight); Reference: man pages for ncat, cowsay, fortune, bash shell

## Project Summary:

> ### Abstract:  
> Using only Linux command line tools and shell I/O redirection, create a simple network server allowing a user to obtain their fortune "Spoken" by a Linux "cow".
> ### Objectives: 
> 1. Understand network applications use streaming data similar to streaming files. 
> 1. Become familiar with the Linux environment and bash shell
> 1. Realize the power of networking is its integration at the application layer to common data use paradigms (streams)
> 1. Learn a bit about TCP/IP ports
> ### Grading:
> 45 pts are available for this project
> ### Course Outcomes: 
> R5 (see syllabus for description of course outcomes)

## Project Description: 

Applications that make use of modern networks must have a *uniform interface* to access and use the network to transmit and receive data. This uniform interface must abstract all of the varied details about the logical and physical network and present familiar constructs the application may use to obtain the desired network services. If these application constructs were familiar to other application constructs, with varied behaviors to represent desired network services, then use of these network services becomes more familiar to the application programmer. 

The uniform interface for networking at the application layer is that of streams (for connection-oriented network services). This interface is not wholly different than the streams interface used for file input/output. In fact, many of the same system calls, data structures, and properties are used between the streams interface for networking and for file I/O. So similar is the streams interface that creating a network version of the much beloved Linux ``cat`` program is straight forward enough.

Consider a program that waits for a network connection on a given network port - more about network ports later - and once a client connects to this port, reads the contents of its standard input and "cat's" it to the client using the streams interface. How exactly is this different than the normal Linux `cat` program? Well, for one, the normal Linux `cat` program does not wait for a network connection, but there isn't much different between a network-based `cat` program and the filesystem-based `cat` program. Both will read from standard input and *concatenate* this stream onto their output. In the case of the filesystem-based `cat` program, either the input and/or output file-descriptors have been associated with a filesystem stream. In the case of the network-based `cat` program, either the input and/or the output file-descriptors have been associated with a network stream. How the two programs function after that is virtually identical - except for the "clean-up" phase before termination of the process.

So let's begin exploring how to construct a trivial network server using simple Linux bash shell commands that can be used by clients on any TCP/IP-based network - such as the global Internet. 

### Linux Commands

The following gives a brief description of the Linux commands you will be using in this project. For more complete descriptions you may type the command `man fortune` (for example) at the command prompt to learn more details about any of the commands briefly described below. 

`fortunate`
: When fortune is run with no arguments, it prints out a random epigram. 
```
$ fortune
Tuesday is the Wednesday of the rest of your life.
```

`cowsay`
: Generates an ASCII picture of a cow saying something provided by he user. If run with no command line arguments, it accepts standard input from the command line. If provided command line arguements, it concatenates each command line argument, separating them by a space, and uses that as its input. In either event, it takes the message provided by the user and prints a picture of a cow saying the given message.
```
$ cowsay Tuesday is the Wednesday of the rest of your life. 
 _________________________________________
/ Tuesday is the Wednesday of the rest of \
\ your life.                              /
 -----------------------------------------
        \   ^__^
         \  (oo)\_______
            (__)\       )\/\
                ||----w |
                ||     ||
```

`ncat`
: Is a simple utility that reads and writes data across network connections, using either the TCP or UDP protocol. It is designed to be a reliable "back-end" tool that can be used directly or easily driven by other programs and scripts. 

|Server Side Shell Instance | Client Side Shell Instance |
|---------------------------|----------------------------|
|echo "Hello World" \| ncat -l -n -p ${NETPORT} | nc localhost ${NETPORT} |
| | `Hello World` |

The `${NETPORT}` is a shell variable set uniquely to each student upon logging into `csdept10.cs.mtech.edu`. The `-l` command line argument to `ncat` tells the program to *listen* for incoming connections (act like a server), while the `-n` command line argument prevents the program from performing *name resolution* through DNS, and the `-p` command line argument is used to specify the *port* on which the program should listen for incoming connections. 

The above commands may be combined using the `bash` shell to provide interesting results. For example, what would the following shell command line produce? 

```
$ fortune | cowsay | ncat -l -n -p ${NETPORT}
```

If you use the command `nc localhost ${NETPORT}` from a second shell instance, you would get something that looks like the following: 

```
    _________________________________________
   / You may be infinitely smaller than some \
   | things, but you're infinitely larger    |
   \ than others.                            /
    -----------------------------------------
           \   ^__^
            \  (oo)\_______
               (__)\       )\/\
                   ||----w |
                   ||     ||   
```

Your cow's saying will likely be different as every time the `fortunate` command is executed, it produces a different random epigram. 

Congratulations! 

You have just created a client and server exchange using Linux command line tools. It should not be lost that the system on which the `ncat` command is executed and the system on which the `nc` command is executed could be on opposite sides of the planet, so long as these two applications are *reachable* through a shared TCP/IP network - such as the Internet - and so long as you replace `localhost` with the IP address (or domain name) of the host running the `ncat` process, you will have successfully transmitted an ASCII-based cow saying a random epigram from the `fortunate` command. 

But, there is a probelm ! ! 

As soon as the `nc` process terminates, the `ncat` process also terminates. That makes this solution a one-time use client and server interaction. That's not very useful. There needs to be a way for multiple invocations of `nc` to connect to the `ncat` process and retrieve the cow message. Thankfully, the `ncat` command has a few more options to help:

`-k option`
: causes the `ncat` program to not terminate after closing its stream to the client. This allows the process to continue to provide services to many subsequent client requests. 

`-c option`
: causes the `ncat` program to take whatever is left on the command line and treat it as a command to execute in a new instance of a shell *for every new connection that is made to the service*.

Therefore, our new version of the `ncat` command becomes, 

```
$ ncat -l -k -n -p ${NETPORT} -c "fortune | cowsay"
```

Eureka! It works. 

Now, multiple instances of `nc localhost ${NETPORT}` will get a cow saying a pithy witticism thanks to the `fortune` command. 

In order to stop the `ncat` process, execute the `CTRL-C` combination keystroke in the shell running the process. 

### Shell Variables and Expression Evaluation

You likely understand that Linux shells - like `bash` - provide variables to store strings and numbers and the like. So, for example, 

```
$ foo='bar'
$ echo ${foo}
bar
```

Shell variables may also hold collections as well. For example, the `cowsay` command has a command line option `-f` the parameter of which is to a `cow file` that changes the cow character into a different one. There are lots of these files.  For example,

```
   $ fortune | cowsay -f dragon
    ______________________________________
   / You will live a long, healthy, happy \
   \ life and make bags of money.         /
    --------------------------------------
         \                    / \  //\
          \    |\___/|      /   \//  \\
               /0  0  \__  /    //  | \ \    
              /     /  \/_/    //   |  \  \  
              @_^_@'/   \/_   //    |   \   \ 
              //_^_/     \/_ //     |    \    \
           ( //) |        \///      |     \     \
         ( / /) _|_ /   )  //       |      \     _\
       ( // /) '/,_ _ _/  ( ; -.    |    _ _\.-~        .-~~~^-.
     (( / / )) ,-{        _      `-.|.-~-.           .~         `.
    (( // / ))  '/\      /                 ~-. _ .-~      .-~^-.  \
    (( /// ))      `.   {            }                   /      \  \
     (( / ))     .----~-.\        \-'                 .~         \  `. \^-.
                ///.----..>        \             _ -~             `.  ^-`  ^-_
                  ///-._ _ _ _ _ _ _}^ - - - - ~                     ~-- ,.-~
                                                                     /.-~
```

All of these cow files are located in a single directory `/usr/share/cowsay/cows`. Let's say you wanted to store the pathname to each of these files in a single variable that is a collection. You could issue the following shell command, 

```
   $ files=(/usr/share/cowsay/cows/*)
   $ echo ${files}
   /usr/share/cowsay/cows/apt.cow
   $ echo ${#files[@]}
   52
   $ echo ${files[@]}
   /usr/share/cowsay/cows/apt.cow /usr/share/cowsay/cows/beavis.zen.cow 
   /usr/share/cowsay/cows/bong.cow /usr/share/cowsay/cows/bud-frogs.cow 
   /usr/share/cowsay/cows/bunny.cow /usr/share/cowsay/cows/calvin.cow
   /usr/share/cowsay/cows/cheese.cow /usr/share/cowsay/cows/cock.cow 
   /usr/share/cowsay/cows/cower.cow /usr/share/cowsay/cows/daemon.cow 
   /usr/share/cowsay/cows/default.cow /usr/share/cowsay/cows/dragon-and-cow.cow
   /usr/share/cowsay/cows/dragon.cow /usr/share/cowsay/cows/duck.cow 
   /usr/share/cowsay/cows/elephant.cow /usr/share/cowsay/cows/elephant-in-snake.cow 
   /usr/share/cowsay/cows/eyes.cow /usr/share/cowsay/cows/flaming-sheep.cow
   /usr/share/cowsay/cows/ghostbusters.cow /usr/share/cowsay/cows/gnu.cow 
   /usr/share/cowsay/cows/head-in.cow /usr/share/cowsay/cows/hellokitty.cow 
   /usr/share/cowsay/cows/kiss.cow /usr/share/cowsay/cows/kitty.cow
   /usr/share/cowsay/cows/koala.cow /usr/share/cowsay/cows/kosh.cow 
   /usr/share/cowsay/cows/luke-koala.cow /usr/share/cowsay/cows/mech-and-cow.cow 
   /usr/share/cowsay/cows/meow.cow /usr/share/cowsay/cows/milk.cow
   /usr/share/cowsay/cows/moofasa.cow /usr/share/cowsay/cows/moose.cow 
   /usr/share/cowsay/cows/mutilated.cow /usr/share/cowsay/cows/pony.cow 
   /usr/share/cowsay/cows/pony-smaller.cow /usr/share/cowsay/cows/ren.cow
   /usr/share/cowsay/cows/sheep.cow /usr/share/cowsay/cows/skeleton.cow 
   /usr/share/cowsay/cows/snowman.cow /usr/share/cowsay/cows/sodomized-sheep.cow 
   /usr/share/cowsay/cows/stegosaurus.cow /usr/share/cowsay/cows/stimpy.cow
   /usr/share/cowsay/cows/suse.cow /usr/share/cowsay/cows/three-eyes.cow 
   /usr/share/cowsay/cows/turkey.cow /usr/share/cowsay/cows/turtle.cow 
   /usr/share/cowsay/cows/tux.cow /usr/share/cowsay/cows/unipony.cow
   /usr/share/cowsay/cows/unipony-smaller.cow /usr/share/cowsay/cows/vader.cow 
   /usr/share/cowsay/cows/vader-koala.cow /usr/share/cowsay/cows/www.cow
```

You can see that depending on the syntax used you can evaluate the shell variable in different contexts to obtain the value of the first element, the total number of entries in the collection, and every entry in the collection.

Similarly, there a number of different ways to execute commands - one of the main responsibilities of the shell. For example,

```
   $ echo "At the $(echo Tone, the time will be $(date))"
   At the Tone, the time will be Sun Aug 26 19:12:51 MDT 2018
```

The `$(cmd)` construct gets executed first and the result replaces the command expansion. These may be nested any depth. Therefore, the above command would first obtain a date string by executing `date` which would then replace the `$(date)` command expansion. Then, the `$(echo ...)` would be expanded by executing the `echo` command and `Tone, the time will be` *date string* would replace the command expansion. Finally, the outermost `echo` command would execute. 

These shell features can be combined to create desired behaviors from commands that are quite surprising albeit syntactically unpleasant. For example,

```
$ cowsay $(fortune)
```

is an equivalent replacement - in terms of behavior - for the previous 

```
$ fortune | cowsay
```

How are these the same? How are they different? Is one *better* than the other? Why? 

What happens if you execute the command, 

```
$ ncat -l -k -n -p ${NETPORT} -c 'cowsay $(fortune)'
```

Is this behavior the same as, 

```
$ ncat -l -k -n -p ${NETPORT} -c "fortune | cowsay"
```

Why? or Why not? How are these two commands different? 

_Lab Environment_

The course lab and project environment is a Linux virtual machine. The machine name is `csdept10.cs.mtech.edu`. You may access this virtual machine using any available `ssh` (secure shell) client software. On Microsoft Windows-10, you can open a command window and use the built-in `ssh` client software:

Press the `Windows Key` and type `cmd` and then hit `enter`

in the command window, you should then type,

`ssh pcurtiss@csdept10.cs.mtech.edu`

and hit `enter`, where `pcurtiss` is replaced with your username. Your username is the first part of your email address, stripped of any trailing numbers. 

You will then be challenged for your password, which you will not see as you type the characters of your password for the sake of privacy. Your password is same password you used on `lumen.cs.mtech.edu` or your student ID if your password has been reset by your instructor. 

Once you have successfully logged in, you will be presented with your Linux prompt. The default prompt from the system will look something similar to the following: 

`pcurtiss@csdept10: ~$`

Once logged into `csdept10`, please execute the following commands:

```
mkdir -p ~/CSCI466/Labs
mkdir -p ~/CSCI466/Projects
```

The tilde character "`~`" is shorthand for your home directory path (`/home/students/jqpublic`, for example). The `mkdir` command creates directories within the filesystem. The `-p` command line option to the `mkdir` command instructs the command to create any intermediate (sub)directories in the path provided to the command (if needed). Therefore, the first command will create the directory `CSCI466` and then create the subdirectory `Labs` inside of the `CSCI466` directory. The second command will not make the `CSCI466` directory, as it was already created in the previous step, but will create a subdirectory `Projects` in the `CSCI466` directory. 

All of the lab assignments should be placed in the `~/CSCI466/Labs` directory, while all of the project assignments should be placed in the `~/CSCI466/Projects` directory. You may see the contents of any directory by using the `ls` command:

```
ls ~/CSCI466
Labs Projects public_html
```

_Git Global Configuration_

`git` is a powerful set of tools for managing source code in a collaborative environment. The broad area in which `git` falls is that of *source control management* (SCM). You will use `git` to retrieve and submit your labs and projects in this, any many other CS Courses here at Montana Tech. The system works by maintaining *repositories* that contain the files you wish the system to manage. Within a repository, the `git` system creates *commit sets* containing only the differences between different commit sets. It stores these commit sets in a highly efficient manner, and can apply them *forward* or *backward* to generate the different *versions* of the files that comprise your repository. 

Repositories can be either *local* or *remote*. It is the remote repositories that are typically *shared* between different software developers working on a project. The remote repository represents the collaborative work of all the independent developers who have contributed to the software project. For your CS Courses in which you will use the `git` SCM, your remote repositories will be managed by the CS Department's `GitLab` server.

In order to work with the `git` SCM system, you must first provide it just a few global configuration parameters, and then you must secure your communications between your `csdept10.cs.mtech.edu` - which is managing your local repository - and `gitlab.cs.mtech.edu` - which is managing your remote repository. 

To configure the global settings for the `git` SCM system on `csdept10.cs.mtech.edu`, execute the following: 

```
git config --global user.name "Phillip Curtiss"
git config --global user.email pcurtiss@mtech.edu
git config --global push.default matching
git config --global core.editor nano
git config --list
user.name=Phillip Curtiss
user.email=pcurtiss@mtech.edu
push.default=matching
core.editor=nano
```

Communication between the Linux environment and the `gitlab.cs.mtech.edu` server can be secured if you use `ssh` keys to encrypt the exchanged information. In order to do this, both the Linux environment (private `ssh` key) and the `GitLab` server (public `ssh` key) must have parts of the same `ssh` key. The first step is to generate an `ssh` key pair in the Linux environment that you wish to use with `GitLab`:

```
ssh-keygen -t rsa
Generating public/private rsa key pair. 
Enter file in which to save the key (/home/faculty/pcurtiss/.ssh/id_rsa):
Created directory '/home/faculty/pcurtiss/.ssh'.
Enter passphrase (empty for no passphrase):
Enter same passphrase again:
Your identification has been saved in /home/faculty/pcurtiss/.ssh/id_rsa.
Your public key has been saved in /home/faculty/pcurtiss/.ssh/id_rsa.pub.
...
```

_Configuring Your GitLab Account_

You must now configure your `GitLab` environment with your public `ssh` key and then `fork` today's project into your own account - creating a remote repository for today's project. Then, you will `clone` the remote repository on the Linux system to create the local repository in which you will do your work. When completed, you will then `push` the commit sets in your local repository back to the remote repository where your instructor and/or TAs will review and grade your work. 

Open a web browser on your local workstation and navigate to the following url [https://gitlab.cs.mtech.edu](https://gitlab.cs.mtech.edu). Sign-in to your `GitLab` account using the credentials you configured from the *invite* you were sent by the system - you may also need to acknowledge the *terms and conditions* of use of the server. 

Once logged into the `GitLab` server, you  will notice a menu bar (with a blue background) at the very top of the browser's content area. To the far right of the menu bar will be an icon that is a pull-down menu. `Left-click` on this icon to open the pull-down menu and then select the `settings` option from the menu.

On the menu to the left you should see an `SSH Keys` item, `left-click` that menu item. This will show you all of the `ssh` keys you have associated with your `GitLab` account. Now, go back to your `csdept10.cs.mtech.edu` terminal window and type following command:

```
cat ~/.ssh/id_rsa.pub
...
```

and you will see on the terminal window the contents of the `id_rsa.pub` file. Highlight and copy to the clipboard the contents shown in the terminal window from this file - and only this file. Then, go back to the browser window and past the clipboard contents into the `Key` window, and then `left-click` the `Add Key` button to add this public `ssh` key to your `GitLab` account.

_Forking Today's Lab Repository_

On the top menu bar you should see a `Projects` pull-down menu toward the left. `Left-click` on this pull-down menu and select `Your Projects` to see a list of projects to which you have access, as well as your own projects - projects in your own `GitLab` account. One of these projects should look something like the following:

```
Networks / F20 CSCI466 / cowsay-server
```

This project, to which you have access, is in the course (Networks) repository for this term's (F20 CSCI466) offering. In order to be able to modify and work with the files in this repository you must get a copy of this project into your own account on `GitLab`. Making a remote copy from one account on `GitLab` to another account on `GitLab` is called `forking`. 

To fork today's project repository, `left-click` on the the course project shown above. This will show you a summary of the project, with some of the files shown in a details list, but at the top, to the right of the project name, you should see a few buttons - [A bell icon], [A star icon], and a [Fork] button. `Left-click` on the `fork` button and then select your account as the destination account into which to place the `forked` copy of the project repository - it will take a few minutes for the fork to complete. 

Once completed, you will notice that you are placed in the project contents, but at the top left, the path now looks like,

```
jqpublic / cowsay-server 
```

or similar, with your username in the path. This indicates the remote repository has been copied into your account. 

The next step is to add your instructor (and any TAs) as members to this project so they can see the project contents, review the project, and provide a grade for your work. The menu on the left should have a `Members` item. `Left-click` the `Members` menu item on the left and you should be presented with a form. Make sure the `Invite Members` tab at the top is selected and then enter your instructor's (and any TA's) name or email in the `GitLab member or Email address` form field. When you have added the instructor (and any TAs) to this field, `left-click` on the `Choose a role permission` and select `Developer` from the options; you should leave the `Access expiration date` blank. Then `left-click` the `invite` button to add them as members to the project. 

Lastly, `left-click` on the `Project Overview` item in the left menu to display the project information. To the right should be a `blue` button labeled `clone`. `left-click` this button and select the `copy url clipboard` icon to the right of the `ssh` url item to copy the url to the workstation's clipboard. 

_Obtaining Project Files_

Now you are ready to copy the remote project repository from your account on the `GitLab` server to your account on `csdept10.cs.mtech.edu`. 

Switch back to the Linux window and type the following: 

```
cd ~/CSCI466/Projects
git clone git@gitlab.cs.mtech.edu:pcurtiss/f20/cowsay-server.git
```

where the specific url is from you pasting the `ssh url` into the window that you copied in your lab step from working with the `GitLab` server - the one shown above is just an example. When you hit `enter` you will be challenged for the passphrase you used when you created your `ssh` key. Enter this passphrase and hit `enter`. Git will now clone (or copy) the remote project repository from your account on `GitLab` to a local repository in your account on `csdept10.cs.mtech.edu` and link the two repositories, such that as you make changes to your local repository, you can `push` these changes back to the remote repository as needed, to update the shared (with your instructor and any TAs) repository on the `GitLab` server. 

Once the `clone` has completed, you should have a new (sub)directory in your `~/CSCI466/Projects` directory named `cowsay-server`, which you can see by using the `ls` command. 

_Specific Project Tasks_

Please perform the following activities in the completion of the project: 

1. Review the code provided in the project. 
1. If you were to execute the following command, 
```
$ fortune | cowsay | ncat -l -k -n -p ${NETPORT}
```
as a server instance, and then use the command, 
```
$ nc localhost ${NETPORT}
```
as a client instance - it will obtain a cow saying the first time executed, but all subsequent times it simply hangs, until you terminate the process. Explain why this is the case. In particular, if you leave off the `-k` option, the `nc` command will work, but the `ncat` process will terminate. Why does adding the `-k` option change this behavior? 

3. Explain in detail what the following shell commands do when the second command is executed repeatedly:
```
$ files=(/usr/share/cowsay/cows/*)
$ echo ${files[$RANDOM%${#files[@]}]} 
```

4. Modify the second line (the one with the echo command) to only output the filename and not the entire pathname. Hint: There are many ways to accomplish this. You can explore the command `basename` or you could explore the variable pattern modifiers - e.g. ${foo##*/}

5. Use the modified version of your echo command to generate a random cow files to use by the command `cowsay`. For example,
```
$ fortune | cowsay -f $(...your echo command here...)
```

6. Rewrite the previous command to get rid of the need to use the `|` and instead, use a second command expansion to provide random cows and random fortunes. For example,
```
$ cowsay -f $(...your echo command here...) $(...your fortune command here...)
```

7. Is the following command correct for providing a network service that result in a cow saying a random fortune? If so, explain the syntax required to make this happen? If not, which desired behavior is violated and why? 
```
$ ncat -l -k -n -p ${NETPORT} -c "cowsay \$(fortune)"
```

8. What does the following do, and explain why the syntax is require to provide the demonstrated behavior.
```
$ ncat -l -k -n -p ${NETPORT} -c "cowsay -f \$(bash -c 'files=(/usr/share/cowsay/cows/*) && \
> echo \${files[\$RANDOM%\${#files[@]}]##*/}') \$(fortune)"
```

*NOTE:* The backslash at the end of the first line is a continuation character, informing the shell that the rest of the command will continue on the next line. The shell then prints the `>` instead of the normal `$` prompt to indicate it is waiting for input to continue to command previously started. You do not need to type the backslash if you do not want to - I used it here since the entire command will not fit on one line of the paper. 

### Providing Answers

Please provide written answers (with examples) in a text file in the root of the project directory called ``report.txt``. Make sure you put your name, date, course number and name at the very top of the file. 
