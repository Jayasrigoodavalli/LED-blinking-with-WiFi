//Load WiFi Library
#include<WiFi.h>
//Replace with your network credentials
const char* ssid="chandu";
const char* password="chandu26";
//Set web server port number to 80
WiFiServer server(80);
//Variable to store the HTTP request
String header;
//Auxilar variables to store the current output state
String output2State="Off";
//Assign output variables to GPIO pins
const int output2=2;
void setup()
{
  // put your setup code here, to run once:
    Serial.begin(115200);
    //Intialize the output variables as outputs
    pinMode(output2,OUTPUT);
    //Set outputs to LOW
    digitalWrite(output2,LOW);
    //Connect to WiFi network with SSID and password
    Serial.print("Connecting to");
    Serial.println(ssid);
    WiFi.begin(ssid,password);
    while(WiFi.status()!=WL_CONNECTED)
    {
      delay(500);
      Serial.println(".");
    }
    //Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address");
    Serial.println(WiFi.localIP());
    server.begin();
}

void loop()
{
  // put your main code here, to run repeatedly:
  WiFiClient client=server.available();//Listen for incoming details
  if(client) //if a new client connects
  {
    Serial.print("New client.");//print a message out in the serial port
    String currentLine="";
    while(client.connected())
    {
      if(client.available())
      {
        char c=client.read();
        Serial.write(c);
        header+=c;
        if(c=='/n')
        {
          //if the byte is a newline character
          //if the current line is blank,you got two newline characters in a row
          //that's the end of the client HTTP request,so send a response:
          if(currentLine.length()==0)
          {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection:close");
            client.println();
            //Turns the GPIOs on &off
            if(header.indexOf("GET /2/on")>=0)
            {
              Serial.println("GPIO 2 on");
              output2State="on";
              digitalWrite(output2,HIGH);
            }
            else if(header.indexOf("GET /2/off")>=0)
            {
               Serial.println("GPIO 2 off");
              output2State="off";
              digitalWrite(output2,LOW);
            }
             // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 2  
            client.println("<p>GPIO 2 - State " + output2State + "</p>");
            // If the output2State is off, it displays the ON button       
            if (output2State=="off")
            {
              client.println("<p><a href=\"/2/on\"><button class=\"button\">ON</button></a></p>");
            } 
            else 
            {
              client.println("<p><a href=\"/2/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } 
          else 
          { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } 
        else if (c != '\r')
        { 
          // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
