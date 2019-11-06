#!/bin/python

# Name: Xavier Hollingsworth
# Date: 08/11/19
# Course: CS372
# Assignment: Project 2 
# Sources: https://topidesta.files.wordpress.com/2015/10/ptyhon-network-engginering-english-version.pdf
# https://www.geeksforgeeks.org/socket-programming-python/
# http://stackoverflow.com/questions/24196932/how-can-i-get-the-ip-address-of-eth0-in-python
# https://stackoverflow.com/questions/24196932/how-can-i-get-the-ip-address-from-nic-in-python


from socket import *
import sys




# This function validates the arguments that the user runs in the terminal
def validateArgs():

	# Check to make sure the amount of arguments is correct
	if len(sys.argv) < 5 or len(sys.argv) > 6:
		print "Incorrect number of arguments"
		exit(1)

	# Make sure host name is valid (flip)
	elif (sys.argv[1] != "flip1" and sys.argv[1] != "flip2" and sys.argv[1] != "flip3"):
		print "Incorrect host name."
		exit(1)

	# Make sure port number is valid
	elif (int(sys.argv[2]) > 65535 or int(sys.argv[2]) < 1024):
		print "Invalid port number"
		exit(1)

	# Check that correct option is used
	elif (sys.argv[3] != "-l" and sys.argv[3] != "-g"):
		print "Invalid command, Please specify -g or -l."
		exit(1)

	# Check valid port number when -l command is used
	elif (sys.argv[3] == "-l" and (int(sys.argv[4]) > 65535 or int(sys.argv[4]) < 1024)):
		print "Invalid port number with l."
		exit(1)

	# Check valid port number when -g command is used
	elif (sys.argv[3] == "-g" and (int(sys.argv[4]) > 65535 or int(sys.argv[5]) < 1024)):
		print "Invalid port number."
		exit(1)




# This function connects the client to the server
def connectSocket():
	#Initialize variables to arguments
	server = sys.argv[1] + ".engr.oregonstate.edu"
	port = int(sys.argv[2])

	# Create the socket
	clientSocket = socket(AF_INET, SOCK_STREAM)
	clientSocket.connect((server, port))
	print "Client connected to server successfully.\n"
	return clientSocket


# This function sets the socket up so that information can be exchanged between client and server
def setupSocket():
	#Initialize variables to args
	if sys.argv[3] == "-l":
		args = 4
	elif sys.argv[4] == "-g":
		args = 5
	# Initialize port
	port = int(sys.argv[args])
	
	# Now setup the socket
	serverSocket = socket(AF_INET, SOCK_STREAM)
	serverSocket.bind(('', port))
	serverSocket.listen(1)
	clientSocket, address = serverSocket.accept()
	return clientSocket


# Gets the IPaddress from the server
# Taken directly from this source: https://stackoverflow.com/questions/24196932/how-can-i-get-the-ip-address-from-nic-in-python
def getIPAddress():
	s = socket(AF_INET, SOCK_DGRAM)
	s.connect(("8.8.8.8", 80))
	return s.getsockname()[0]

# Open a file to write to
def getFile(dataSocket):
	f = open(sys.argv[4], "w")
	buff = dataSocket.recv(1000)

	# Write to file until you reach the end
	while "__done__" not in buff:
		f.write(buff)
		buff = dataSocket.recv(1000)


# Function to get the list of files
def getFileList(dataSocket):
	# Get first file name
	filename = dataSocket.recv(100)

	# Loop until no more filenames need to be retrieved
	while filename != "done":
		print filename
		filename = dataSocket.recv(100)
	


def retrieveData(clientSocekt):
	# Initialize variables to arguments	
        hostname = sys.argv[1]
	hostport = sys.argv[2]
	command = sys.argv[3]
	fileName = sys.argv[4]


	# Communication occurs here
	# Determine what the user wants to do based on the command they used
	if sys.argv[3] == "-l":
		print "Receiving directory structure from ", hostname, ":", hostport
		portNumber = 4
 	elif command == "-g":
		print "Receiving file ", filename, " from ", hostname, ":", hostport
		portNumber = 5

	# Send to port number
	clientSocket.send(sys.argv[portNumber])
	clientSocket.recv(1024)

	if sys.argv[3] == "-g":
		clientSocket.send("g")
	else:
		clientSocket.send("l")

	clientSocket.recv(1024)

        clientSocket.send(getIPAddress())
	clientResponse = clientSocket.recv(1024)

	# Check to make sure proper command was received
	if clientResponse == "bad":
		print "Invalid command"
		exit(1)

	if sys.argv[3] == "-g":
		clientSocket.send(sys.argv[4])
		newResponse = clientSocket.recv(1024)

		# Error message if file is not found
		if newResponse != "File found":
			print "Could not find file."
			return

	# Setup data socket
	clientSocket = setupSocket()

	# Based on command enetered get file or file list
	if sys.argv[3] == "-l":
		getFileList(dataSocket)
	if sys.argv[3] == "-g":
		getFile(dataSocket)


	# And finally close the data socket
	dataSocket.close()



if __name__ == "__main__"
	# Validate the arguments:
	validateArgs()
	
	# Connect to the server
        clientsocket = connectSocket()

	# Exchange data with server
	retrievedata(clientsocket)
