Name: Xavier Hollingsworth
Date: 08/11/2019


# Important notes before starting
Make sure that the client and host programs are running on different hosts. Also make sure that both of the programs are located in different directories.


# Compiling server
To compile the server simply type gcc ftserver.c -o ftserver

# Running server
To run the server type ./ftserver [port number]

# Running client
First give the client executable permissions by typing chmod +x ftclient.py
Run the program by typing python ftclient.py [hostname]  [server port] -l [data port]
or python ftclient.py [hostname] [server port] -g [file name] [data port]
