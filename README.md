# Systems-Programming-CW-2
A Client Server Application in C Using Unix Sockets and Threads

The client offers to the user several options which each call a corresponding function in
the server to request information. These options at the client user interface can be
executed repeatedly, on demand and in any order:-

- Get and display your hardcoded name and student ID from the server prefixed
    with the server IP address. This should be transmitted as a single concatenated
    string by the server.

- Get and display an array of 5 random number in the range 0 to 1000.

- Get and display the “uname” information of the server including OS name,
    release and version. This should be transmitted as an utsname structure.

- Get and display a list of file names for the regular files present on the server in
    its “upload” directory. This can be sent as a single string formatted with a
    suitable separator.
    
The “upload” directory is a simple sub-directory of the current working directory of the
server executable. The above functions should use suitable system calls using the C
standard library (i.e. the GNU C Library) as appropriate. 

File copy enhancement: copy the contents of a file specified by the client user (text or
binary - of any length) from the server “upload” directory to the client executable’s
current working directory. The file must be transferred via a socket. File names should
not be hardcoded into the client code. Better courseworks will handle error conditions
(e.g. if the requested file does not exist) which will be displayed by the client.


The Remote System Information Service Client:


A simple interactive text-based client is to be implemented which will connect to the
server on port 50031 on startup. The options supported through a simple menu interface will 
be the list of functions given above with an additional option to quit the client (the server will keep going).
Note that the client should be interactive and menu-driven.


The Remote File Service and System Information Service Server:


The server must listen for Socket connections on port 50031 and must start a new thread
for each connected client. For each connected client the server will process its requests.
This multithreaded approach allows the server to deal with many clients simultaneously.
The code executed by each thread will be identical. You should use POSIX threads.
Additionally the server should implement a signal handler to gracefully exit, clean up and
display the total server execution time. This should be executed on receipt of <ctrl> + c
or SIGINT.

