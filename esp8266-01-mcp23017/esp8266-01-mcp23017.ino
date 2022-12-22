
// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <Wire.h>

//ESP-01 i2c pins (arduino defaults to 4/5)
#define SDA 0
#define SCL 2

// Replace with your network credentials
const char* ssid     = "TP-Link_AABA";
const char* password = "08065452";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output5State = "off";
String output4State = "off";

// Assign output variables to GPIO pins
const int output5 = 5;
const int output4 = 4;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

// Set your Static IP address
IPAddress local_IP(192, 168, 0, 201);
// Set your Gateway IP address
IPAddress gateway(192, 168, 0, 1);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 0, 1);   //optional
IPAddress secondaryDNS(192, 168, 0, 1); //optional


void setup() 
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    display_Running_Sketch();

    Serial.println("\nI2C Scanner");
    Serial.print("SDA: ");
    Serial.println(SDA);
    Serial.print(" SCL: ");
    Serial.println(SCL);

    Serial.print("Wire.begin("); 
    Serial.print(SCL);
    Serial.print(",");
    Serial.print(SDA);
    Serial.print(")");;

    Wire.begin(SCL,SDA);

    // Initialize the output variables as outputs
    pinMode(output5, OUTPUT);
    pinMode(output4, OUTPUT);
    // Set outputs to LOW
    digitalWrite(output5, LOW);
    digitalWrite(output4, LOW);

    // Configures static IP address
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) 
    {
        Serial.println("STA Failed to configure");
    }

    // Connect to Wi-Fi network with SSID and password
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();
}

void loop() 
{
    WiFiClient client = server.available();   // Listen for incoming clients

    if (client) 
    {                             // If a new client connects,
        Serial.println("New Client.");   // print a message out in the serial port
        String currentLine = "";      // make a String to hold incoming data from the client
        currentTime = millis();
        previousTime = currentTime;

        while (client.connected() && currentTime - previousTime <= timeoutTime) 
        { 
            // loop while the client's connected
            currentTime = millis();         

            if (client.available()) 
            {             // if there's bytes to read from the client,
                char c = client.read();             // read a byte, then
                Serial.write(c);                    // print it out the serial monitor
                header += c;

                if (c == '\n') 
                { 
                    // if the byte is a newline character
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    //
                    if (currentLine.length() == 0) 
                    {
                        writeHtmlPageHeaders( client );
                        checkUpdatedIoValues( header );
                        writeHtmlPageCSS( client );
                        writeHtmlPageData( client );
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

void checkForHeaderCommands( String & header )
{
    if (header.indexOf("GET /i2c/scan") >= 0)
    {



        Serial.println("GPIO 5 on");
        output5State = "on";
        digitalWrite(output5, HIGH);
    }
}


void checkUpdatedIoValues( String & header )
{
    // turns the GPIOs on and off
    if (header.indexOf("GET /5/on") >= 0)
    {
        Serial.println("GPIO 5 on");
        output5State = "on";
        digitalWrite(output5, HIGH);
    }
    else if (header.indexOf("GET /5/off") >= 0)
    {
        Serial.println("GPIO 5 off");
        output5State = "off";
        digitalWrite(output5, LOW);
    } 
    else if (header.indexOf("GET /4/on") >= 0) 
    {
        Serial.println("GPIO 4 on");
        output4State = "on";
        digitalWrite(output4, HIGH);
    }
    else if (header.indexOf("GET /4/off") >= 0) 
    {
        Serial.println("GPIO 4 off");
        output4State = "off";
        digitalWrite(output4, LOW);
    }
}

void writeHtmlPageData( WifiClient & client )
{
    client.println("<body><h1>Freds ESP8266 MCP23017 Controller</h1>");

    // Display current state, and ON/OFF buttons for GPIO 5  
    client.println("<p>GPIO 5 - State " + output5State + "</p>");
    // If the output5State is off, it displays the ON button       

    if (output5State=="off") 
    {
        client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
    }
    else
    {
        client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
    } 

    // Display current state, and ON/OFF buttons for GPIO 4  
    client.println("<p>GPIO 4 - State " + output4State + "</p>");

    // If the output4State is off, it displays the ON button       
    if (output4State=="off") 
    {
        client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
    }
    else
    {
        client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
    }

    client.println("</body></html>");

    // The HTTP response ends with another blank line
    client.println();
}

//
// so we have 16 additional IO ports over the I2C bus with the MCP23017
// for now lets see if we can light up a few leds first to check outputs
// then we want to read a couple buttons to check inputs
// sounds fun and exciting for sure.  need to find my gvimrc file.  bah.
//
void displayCurrentOutputs( WiFiClient & client )
{

}

void writeHtmlPageHeaders( WifiClient & client )
{
    // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
    // and a content-type so the client knows what's coming, then a blank line:
    //
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();
}

void writeHtmlPageCSS( WifiClient & client )
{
    client.println("<!DOCTYPE html><html>");
    client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client.println("<link rel=\"icon\" href=\"data:,\">");
    // CSS to style the on/off buttons 
    // Feel free to change the background-color and font-size attributes to fit your preferences
    client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
    client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
    client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
    client.println(".button2 {background-color: #77878A;}</style></head>");
}

byte find_i2c_address(void)
{
    byte error = 0;
    byte address = 0;
    int nDevices = 0;

    Serial.println("Scanning...");

    for(address = 1; address < 127; address++ ) 
    {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
            Serial.print("I2C device found at address 0x");

            if (address<16) 
            {
                Serial.print("0");
            }

            Serial.println(address,HEX);
            nDevices++;
        }
        else if (error==4) 
        {
            Serial.print("Unknow error at address 0x");

            if (address<16) 
            {
                Serial.print("0");
            }
            Serial.println(address,HEX);
        }    
    }

    if (nDevices == 0) 
    {
        Serial.println("No I2C devices found\n");
        address = 0;
    }
    else
    {
        Serial.println("done\n");
    }

    return address;        
}

// displays at startup the Sketch running in the Arduino
void display_Running_Sketch (void)
{
    String the_path = __FILE__;
    int slash_loc = the_path.lastIndexOf('/');
    String the_cpp_name = the_path.substring(slash_loc+1);
    int dot_loc = the_cpp_name.lastIndexOf('.');
    String the_sketchname = the_cpp_name.substring(0, dot_loc);

    Serial.print("\nArduino is running Sketch: ");
    Serial.println(the_sketchname);
    Serial.print("Compiled on: ");
    Serial.print(__DATE__);
    Serial.print(" at ");
    Serial.print(__TIME__);
    Serial.print("\n");
}
