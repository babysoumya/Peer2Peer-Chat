# Peer2Peer-Chat
• A terminal-based chat application is implemented where every node in the network can connect and transfer text data among themselves

• Application uses Socket Programming. The communication mode is asynchronous, indicating that a user can enter the message anytime

• Every instance of the app runs a TCP server that listens for incoming connections(used select() syscall to maintain multiple file descriptors)
