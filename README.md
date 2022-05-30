# Food-Delivery
Project developed for Computer Network course at UniPa "Università degli Studi di Napoli Parthenope".
In team with a classmate, Raffaele Sdino.

### Assignment
----
Design and implement a food delivery service according to the following specifications. 
The client connects to the server from which it receives the restaurant list. The user, using the client interface, chooses the restaurant. The server requests a list of foods and drinks that can be ordered from the chosen restaurant and sends it to the client from whom the user places the order. Once the order has been placed, the server forwards it to the restaurant which verifies the availability of riders by sending the request to the connected riders and selecting the first one from which it receives the confirmation. Upon receipt of the confirmation, the server sends the rider's ID to the client and the client's ID to the rider. Once the delivery has been made, the rider sends a notification to the restaurant which in turn forwards it to the server. All communications take place via the server.

# Our implementation of requirements
The architecture used provides the following scheme: 
"Each user registered in the file system accesses the "client.c" application, interfacing as a user with the main server, which when it has to communicate with one of the two restaurant applications becomes in turn a client of the latter, which therefore act as the server of the main server; moreover, the restaurant applications are also servers with respect to the rider applications".
The TCP socket protocol implemented in Linux environment was used for the project.
<img width="864" alt="Schermata 2022-05-30 alle 13 41 09" src="https://user-images.githubusercontent.com/83810152/170985094-b36be714-a5f8-46e5-a17a-66364234c3a1.png">


# General Architecture and Application Protocol
The server manages the connection requests from the client, through the call to the procedure "int pid_t fork (void)" through which the process asks the operating subsystem to generate an identical copy of itself, called child process, through which will manage that specific connection request in a dedicated way. 
The parent and child server processes are therefore executed "simultaneously", and the child process manages the connection with a given client while the parent process can accept new connections. Each new connection spawns a new child process that handles client requests.
It should also be noted that the main server, in the following document, will act both as described above, and in turn as a client towards the two restaurant applications, since it will send the orders placed by the users through the client application, to the server of the chosen restaurant, which in turn will communicate with other clients attributable to the riders.
<img width="875" alt="Schermata 2022-05-30 alle 13 34 25" src="https://user-images.githubusercontent.com/83810152/170984117-79cf5bc9-cc08-4751-9da8-6effbc0e91ef.png">

# Client
Communication between applications takes place using the TCP protocol. 
In fact, a socket of type sock_stream is created for this purpose and information is exchanged with it using the FullRead and FullWrite functions. The information regarding the order, as well as the delivery data are stored in a structure that represents the flow to be exchanged between the applications. 
When the user in "client.c" has completed his order and entered the data for the delivery, the rider in turn in his application finds the presence of an order with the relative data (otherwise he will find a message in which the absence of orders to be carried out is evident). The courier will then have to notify the user of the successful delivery, or the non-compliance of the same. 
The access of the customer and the rider to the corresponding applications is linked to a preparatory login procedure implemented through file system, so that each customer and rider will be provided, after registering, with their own username and password to enter their application.

# Server
The "server.c" application performs different functions based on the application with which it communicates. 
Using a TPC protocol, it creates a channel through which, in fact, it communicates with other applications, that is, both reading and writing operations are performed, using the “FullRead” and “FullWrite” functions.
Furthermore, it was implemented following the model of the so-called “Concurrent” servers, in which N requests for connections are managed by N copies of the parent process, called child processes, which manage the individual connections in a dedicated way.
When the connection request is made by a client, "server.c" behaves like a competing server, finding itself having to communicate both with the client connected to it ("client.c") and with the server of one of the two restaurants chosen by the user ("DeInu.c" or "JapInu.c"). In this case, the operations to be performed are intended to allow the client to carry out and complete an order and to inform the user of his or her delivery (or failure).
To carry out these operations, therefore, "server.c" uses only a TCP protocol and the information that "server.c" exchanges through this flow takes place, as already mentioned, using the "FullRead" and "FullWrite" functions. 
Thus, "server.c" is started before all other applications, and listens for a possible connection from "client.c". When it is revealed and is successfully received, the order placed by the user through "client.c" is communicated by the main server to the server of the chosen restaurant. Secondly, "server.c" will receive from the restaurant application the ID of the courier and the notification of the successful / non-delivery, to be communicated in turn to the "client.c" application. The application of the restaurant chosen by the user, therefore, acts as a server, both towards the applications of the riders, and towards the "server.c" application. 
It remains on hold until there is a connection request either from "server.c" or from one of the two rider applications. Once this connection is revealed from "server.c", and subsequently established successfully, the restaurant application will receive the user's choices relating to the menu. At the same time, the restaurant application also awaits any requests for connections from riders, who, if there are deliveries to be made, receive all the information for the delivery from the restaurant (user ID, order ID, name , surname, address) otherwise they are informed of the absence of orders to be placed.
The courier, however, as already indicated above, will have to provide his ID to the restaurant, so that he can forward it to the main server which in turn will send it to the client to identify himself to the user. Finally, following the order placed, the rider will notify the restaurant of the delivery, and the latter's application will in turn notify "server.c" and then "client.c" of the delivery or non-delivery of the order.
