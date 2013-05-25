/*
 * HTTPPOSTServer.java
 * Author: S.Prasanna
 * @version 1.00 
*/

import java.io.*;
import java.net.*;
import java.util.*;

public class HTTPPOSTServer extends Thread {
	
	static final String HTML_START = 
			"<html>" +
			"<title>HTTP POST Server in java</title>" +
			"<body>";
			
    static final String HTML_END = 
			"</body>" +
			"</html>";
			
	Socket connectedClient = null;	
	BufferedReader inFromClient = null;
	DataOutputStream outToClient = null;
	
			
	public HTTPPOSTServer(Socket client) {
		connectedClient = client;
	}			
			
	public void run() {
		
	  String currentLine = null, postBoundary = null, contentength = null, filename = null, contentLength = null;
	  PrintWriter fout = null;	
	  try {
		
		System.out.println( "The Client "+
        connectedClient.getInetAddress() + ":" + connectedClient.getPort() + " is connected");
            
        inFromClient = new BufferedReader(new InputStreamReader (connectedClient.getInputStream()));                  
        outToClient = new DataOutputStream(connectedClient.getOutputStream());
        
		currentLine = inFromClient.readLine();
        String headerLine = currentLine;            	
        StringTokenizer tokenizer = new StringTokenizer(headerLine);
		String httpMethod = tokenizer.nextToken();
		String httpQueryString = tokenizer.nextToken();
		
		System.out.println(currentLine);
		
		//Interpretting the get Command
        if (httpMethod.equals("GET")) {    
        	System.out.println("GET request");
			currentLine = inFromClient.readLine();
			System.out.println(currentLine);
			sendResponse("d02&40&!240!");
			//sendResponse(0, "fdsafdsafdsa//", false);
		}
		else { //POST request
		    
	  	}//else
	  } catch (Exception e) {
			e.printStackTrace();
	  }	
	}
	
	public void sendResponse (String responseString) throws Exception {
		
		outToClient.writeChars(responseString);
		outToClient.close();
	}
	
			
	public static void main (String args[]) throws Exception {
		
		ServerSocket Server = new ServerSocket (5000, 10, InetAddress.getByName("192.168.43.108"));         
		System.out.println ("HTTP Server Waiting for client on port 5000");
								
		while(true) {	                	   	      	
				Socket connected = Server.accept();
	            (new HTTPPOSTServer(connected)).start();
        }      
	}
}
