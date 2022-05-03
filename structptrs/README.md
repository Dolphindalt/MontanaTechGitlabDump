---
Renderer: GitLab Markdown
Version: 20210109.01
Term: Spring 2021 - CSCI460 Operating Systems
---

| E-Mail Messages Search Tree with C Pointers & Structs  | S21 CSCI460 Operating Systems  |
| :--- | :--- |
| Assigned: **2021-01-07** | Due Date: **2021-01-18** | 
| Abstract: | Design and implement a search tree for E-Mail messages using C language pointers and structs |
| Objectives: | 1. C Language Pointers<br> 2. Structs, Typedefs and Memory Layout <br> 3. Generic Structs and casting as a kind of inheritence <br> 4. Dynamic memory allocation and void pointers in C|
| Grading: 45 pts| A (&ge;41.85); A- (&ge;41.85); <br> B+ (&ge;39.15); B (&ge;37.35); B- (&ge;36.00) <br> C+ (&ge;34.65); C (&ge;32.85); C- (&ge;31.75) <br> D+ (&ge;30.15); D (&ge;28.35); D- (&ge;27) |
| Outcomes: | R8 (CS: 5-1, 2, 5-a-4; SE: 1, III-1-2-1) <br> (see syllabus for description of course outcomes)|

## Project Description:

You must design and implement a program using C structures, pointers, and dynamic memory allocation to read in e-mail messages from a text file, build a search tree from these messages, and then perform and in-order traversal to output the messages in date order. 

### Project  Input

E-mail messages will be in a file of the following format: 

```
n
From: <user>@<host>
To: <user>@<host>
Date: <unix date/time format>
<body>
<blank line>
...
```

### Project Output

Your output should simply be the messages printed out in date order from performing an in-order traversal of the search tree. 

## Specific Project Tasks:

The following tasks should be completed to obtain the maximum points available for this assignment.

1. Obtaining your project files  
   1. Perform the following tasks on the Department GitLab Server:  
      1. Login to the Department GitLab server [gitlab.cs.mtech.edu](https://gitlab.cs.mtech.edu) with your email address and your password. 
      1. Select the project operating-systems/s21-csci460/structptrs> from the offering of this course.
      1. Click on the **fork** button to create a copy of this project under your own GitLab account.
      1. Go to the **project settings** menu and then the **members** option and add your instructor and any teaching assistants as developers to your copy of the project. 
      1. Lastly, go back to this project under your account and click the icon to the right of the project URL. This will copy the project URL to the clipboard so you may paste it in the next sequence of steps. 
   1. Perform the following tasks on the Department Linux server `csdept11.cs.mtech.edu`. 
      1. Login to the Department's Linux server with your credentials. 
      1. Execute the commands 
      ```
      mkdir -p ~/CSCI460/Projects
      cd ~/CSCI460/Projects
      ```
      3. Execute the command `git clone <url>`, where `<url>` is pasted from the clipboard.
      1. Change into the directory created by the `git clone` command to perform the rest of the tasks for this project. 
1. Review the code provided in the project. 
1. Review the `main.c` file and modify as necessary to implement the testing of the functionality of your code. 
1. Your code should implement, or finish implmenting, the following:  
   1. Functions to read in the messages from standard input and create the tree nodes, and then insert these nodes into a search tree correctly based on message date. 
   1. Functions related to in-order traversal to print out the messages in date order. 
   1. You should make use of the structs provided as much as possible, but if you desire to change them, make sure you document and then explain the reason for the change(s). You should feel free to create additional structs as needed to help with the project tasks. 
   1. You should try to allocate from the heap as much as possible. 
   1. Pay attention to how best to use generic structs and void pointers to ease the number and complexity of functions you need to write. 

## Project Grading: 

The project must compile without error (ideally without warnings) and should not fault upon execution. All exceptions should be caught if thrown and handled in a rational manner. Grading will follow the *project grading rubric* shown below: 

| Attribute (Pts) | Exceptional (1) | Acceptable (0.8) | Amateur (0.7) | Unsatisfactory (0.6) | 
| :---:           | :---            | :---             | :---          | :---                 |
| Specification (10) | The program works and meets all of the specifications. | The program works and produces correct results and displays them correctly. It also meets most of the other specifications. | The program produces correct results, but does not display them correctly. | The program produces incorrect results. |
| Readability (10) | The code is exceptionally well organized and very easy to follow. | The code is fairly easy to read. | The code is readable only by someone who knows what it is supposed to be doing. | The code is poorly organized and very difficult to read. | 
| Reusability (10) | The code only could be reused as a whole or each routine could be reused. | Most of the code could be reused in other programs. | Some parts of the code could be reused in other programs. | The code is not organized for reusability. | 
| Documentation (10) | The documentation is well written and clearly explains what the code is accomplishing and how. | The documentation consists of embedded comments and some simple header documentation that is somewhat useful in understanding the code. | The documentation is simply comments embedded in the code with some simple header comments separating routines. | The documentation is simply comments embedded in the code and does not help the reader understand the code. | 
| Efficiency (5) | The code is extremely efficient without sacrificing readability and understanding. | The code is fairly efficient without sacrificing readability and understanding. | The code is brute force and unnecessarily long. | The cod eis huge and appears to be patched together. | 
| Delivery (total) | The program was delivered on-time. | The program was delivered within a week of the due date | The program was delivered within 2-weeks of the due date. | The code was more than 2-weeks overdue. | 

### Example: 

The *delivery* attribute weights will be applied to the total score from the other attributes. If a project scores 36 points total for the sum of *specification*, *readability*, *reusability*, *documentation*, and *efficiency* attributes, but was turned in within 2-weeks after the due date, the project score would be $`36\cdot 0.7 = 25.2`$. 

## Collaboration Opportunities: 

This project provides *no collaboration opportunities* for the students. Students are expected to design and implement an original solution specific in this project description. Any work used that is not original should be cited and properly references in both the code and in any accompanying write-up. Failure to cite code that is not original may lead to claims of academic dishonesty against the student - if in doubt of when to cite, see your instructor for clarification. 


  
