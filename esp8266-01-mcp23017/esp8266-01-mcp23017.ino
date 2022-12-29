//==============================================================================
// super simple example web page for ESP8266-01 board connected to
// MCP23017 16 port IO expander.  
//
// Playing with some led outputs and adding stuff as I go.
//
// Fred Potmesil
// December 22, 2022
//
//==============================================================================

//
// simple conditional to allow for easy web page output testing
// separate from the board and mcp23017 code 
// there really has to be a better way to just dump html code for testing, right??
// being new I just do not know the 'real' way to do this hopefully, this is klunky
//
#define RUN_ON_ESP8266 1

#if RUN_ON_ESP8266
#include <ESP8266WiFi.h>
#include <Wire.h>

// ESP8266-01 i2c pins (arduino defaults to 4/5)
#define SDA 0
#define SCL 2

byte checkForHeaderCommands(
        const String & header,
        String & output );

byte scan_I2C_for_mcp23017(
        String & output,
        const bool printSerial = false);

byte checkUpdatedIoValues( 
        const String & header,
        String & output );

void writeHtmlPageData( 
        WiFiClient & client,
        const String & commandOutput,
        const String & ioOutput );

void displayCurrentOutputs(
        WiFiClient & client );

void writeHtmlPageHeaders(
        WiFiClient & client );

void writeHtmlPageCSS(
        WiFiClient & client );


// Replace with your network credentials
const char* ssid     = "TP-Link_AABA";
const char* password = "08065452";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// byte stores the I2C address
byte i2cAddress = 0;
//
// store the output states for Port A 0-7
//
int outputStates[8] = {0,1,0,1,0,1,0,1};

//
// store the input states for Port B 0-7
//
int inputStates[8] = {0,0,0,0,0,0,0,0};


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
                        String commandOutput = "";
                        String ioOutput = "";
                        byte address = checkForHeaderCommands( header, commandOutput );

                        if( 0 == address )
                        {
                            checkUpdatedIoValues( header, ioOutput );
                        }

                        writeHtmlPageHeaders( client );
                        writeHtmlPageCSS( client );
                        writeHtmlPageData( client, commandOutput, ioOutput );
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
#else 
#include <iostream>
#include <string>

int8_t checkForHeaderCommands(
        const std::string & header,
        std::string & output );

int8_t checkUpdatedIoValues( 
        const std::string & header,
        std::string & output );

void writeHtmlPageData( 
        const std::string & commandOutput,
        const std::string & ioOutput );

void displayCurrentOutputs( void );
void writeHtmlPageHeaders( void );
void writeHtmlPageCSS( void );

int8_t scan_I2C_for_mcp23017(
        std::string & output,
        const bool printSerial = false );

void display_Running_Sketch( std::string & output );


// Variable to store the HTTP request
std::string header;

// byte stores the I2C address
int8_t i2cAddress = 0;

//
// store the output states for Port A 0-7
//
int outputStates[8] = {0,1,0,1,0,1,0,1};

//
// store the input states for Port B 0-7
//
int inputStates[8] = {0,0,0,0,0,0,0,0};

std::string output5State = "off";
std::string output4State = "off";

// Assign output variables to GPIO pins
const int output5 = 5;
const int output4 = 4;

int main( 
        int argc, 
        const char * argv[] )
{
    std::string commandOutput = "";
    std::string ioOutput = "";
    int8_t address = checkForHeaderCommands( header, commandOutput );

    if( 0 == address )
    {
        checkUpdatedIoValues( header, ioOutput );
    }

    writeHtmlPageHeaders();
    writeHtmlPageCSS();
    writeHtmlPageData( commandOutput, ioOutput );

}
#endif

#if RUN_ON_ESP8266
byte checkForHeaderCommands(
        const String & header,
        String & output )
{
    if( header.indexOf("Get /scanI2C") >= 0)
    {
        return scan_I2C_for_mcp23017(output);
    }
    else if( header.indexOf("Get /sketch") >= 0)
    {
        display_Running_Sketch(output);
    }

    return 0;
}
#else
int8_t checkForHeaderCommands(
        const std::string & header,
        std::string & output )
{
    if( header.find("Get /scanI2C") != std::string::npos)
    {
        return scan_I2C_for_mcp23017(output);
    }
    else if( header.find("Get /sketch") != std::string::npos)
    {
        display_Running_Sketch(output);
    }

    return 0;
}
#endif

#if RUN_ON_ESP8266
byte checkUpdatedIoValues( 
        const String & header,
        String & output )
{
    // turns the GPIOs on and off
    if (header.indexOf("GET /1/on") >= 0)
    {
        Serial.println("GPIO 1 on");
        //output5State = "on";
        outputStates[0] = 1;

        digitalWrite(output5, HIGH);
    }
    else if (header.indexOf("GET /1/off") >= 0)
    {
        Serial.println("GPIO 1 off");
        //output5State = "off";
        outputStates[0] = 0;
        digitalWrite(output5, LOW);
    } 
    else if (header.indexOf("GET /2/on") >= 0) 
    {
        Serial.println("GPIO 2 on");
        //output4State = "on";
        outputStates[1] = 1;
        digitalWrite(output4, HIGH);
    }
    else if (header.indexOf("GET /2/off") >= 0) 
    {
        Serial.println("GPIO 2 off");
        //output4State = "off";
        outputStates[1] = 0;
        digitalWrite(output4, LOW);
    }

    return 0;
}
#else
int8_t checkUpdatedIoValues( 
        const std::string & header,
        std::string & output )
{
#if 0 // not really relevant to the html output yet.
    // turns the GPIOs on and off
    if (header.indexOf("GET /1/on") >= 0)
    {
        Serial.println("GPIO 1 on");
        //output5State = "on";
        outputStates[0] = 1;

        digitalWrite(output5, HIGH);
    }
    else if (header.indexOf("GET /1/off") >= 0)
    {
        Serial.println("GPIO 1 off");
        //output5State = "off";
        outputStates[0] = 0;
        digitalWrite(output5, LOW);
    } 
    else if (header.indexOf("GET /2/on") >= 0) 
    {
        Serial.println("GPIO 2 on");
        //output4State = "on";
        outputStates[1] = 1;
        digitalWrite(output4, HIGH);
    }
    else if (header.indexOf("GET /2/off") >= 0) 
    {
        Serial.println("GPIO 2 off");
        //output4State = "off";
        outputStates[1] = 0;
        digitalWrite(output4, LOW);
    }
#endif

    return 0;
}
#endif


#if RUN_ON_ESP8266
void writeHtmlPageData( 
        WiFiClient & client,
        const String & commandOutput,
        const String & ioOutput )
{
    client.println("<body><h1>ESP8266-01 MCP23017 Controller</h1>");

    //
    // first check the I2C address and output depending on whether we 
    // have successfully initialized the MCP23017 chip yet.
    // still not sure this will work at all, getting exciting...
    //
    client.println("<table id=\"outputs\">\n");
    client.println("<tr>\n<th colspan=\"2\">MCP23017 Address " + 
            (i2cAddress ? String(i2cAddress) : "Unknown") + "</th>\n</tr>\n" );

    client.println("<tr>\n<td>Scan I2C Bus</td>\n");
    client.println("<td><a href=\"/scanI2C\"><button class=\"button2\">SCAN</button></a></td>\n</tr>\n");

    client.println("<tr>\n<td>Dump Application Info</td>\n");
    client.println("<td><a href=\"/info\"><button class=\"button2\">INFO</button></a></td>\n");
    client.println("</tr>\n</table>\n\n");


    client.println("<p><table id=\"outputs\">\n");
    client.println("<tr>\n<th>Output Port and Number</th>\n");
    client.println("<th>Current State</th>\n</tr>\n");

    for( int i = 0; i < 8; i++ )
    {
        //
        // Using MCP23017 Port A 0-7 for led outputs for simplicity.
        //
        client.println("<tr>\n<td>Port A " + String(i) + "</td>\n");

        if( outputStates[i] ) 
        {
            client.println("<td><a href=\"/" + String(i+1) +
                    "/on\"><button class=\"button2\">ON</button></a></td>\n</tr>\n");
        }
        else
        {
            client.println("<td><a href=\"/" + String(i+1) + 
                    "/off\"><button class=\"button\">OFF</button></a></td>\n</tr>\n");
        } 
    }

    client.println("</table>\n</body>\n</html>");
    // End HTTP response with blank line
    client.println();
}
#else
void writeHtmlPageData( 
        const std::string & commandOutput,
        const std::string & ioOutput )
{
    std::cout << "<body><h1>ESP8266-01 MCP23017 Controller</h1>" << std::endl;
    //
    // first check the I2C address and output depending on whether we 
    // have successfully initialized the MCP23017 chip yet.
    // still not sure this will work at all, getting exciting...
    //
    std::cout << "<table id=\"outputs\">\n" << std::endl;
    std::cout << "<tr>\n<th colspan=\"2\">MCP23017 Address " <<
            (i2cAddress ? std::to_string(i2cAddress) : "Unknown") << "</th>\n</tr>\n" << std::endl;

    std::cout << "<tr>\n<td>Scan I2C Bus</td>\n" << std::endl;
    std::cout << "<td><a href=\"/scanI2C\"><button class=\"button2\">SCAN</button></a></td>\n</tr>\n"
        << std::endl;

    std::cout << "<tr>\n<td>Dump Application Info</td>\n" << std::endl;
    std::cout << "<td><a href=\"/info\"><button class=\"button2\">INFO</button></a></td>\n" 
        << std::endl;;
    std::cout << "</tr>\n</table>\n\n" << std::endl;

    std::cout << "<p><table id=\"outputs\">\n" << std::endl;
    std::cout << "<tr>\n<th>Output Port and Number</th>\n" << std::endl;
    std::cout << "<th>Current State</th>\n</tr>\n" << std::endl;

    for( int i = 0; i < 8; i++ )
    {
        //
        // Using MCP23017 Port A 0-7 for led outputs for simplicity.
        //
        std::cout << "<tr>\n<td>Port A " << i << "</td>\n" << std::endl;

        if( outputStates[i] ) 
        {
            std::cout << "<td><a href=\"/" << (i+1) <<
                    "/on\"><button class=\"button2\">ON</button></a></td>\n</tr>\n" << std::endl;
        }
        else
        {
            std::cout << "<td><a href=\"/" << (i+1) <<
                    "/off\"><button class=\"button\">OFF</button></a></td>\n</tr>\n" << std::endl;
        } 
    }

#if 0
    // Display current state, and ON/OFF buttons for GPIO 5  
    std::cout << "<div>\n<div class=\"titlediv\">GPIO 5 - State " + output5State + "</div>\n" 
        << std::endl;
    // If the output5State is off, it displays the ON button       

    if (output5State=="off") 
    {
        std::cout << "<div class=\"buttondiv\"><a href=\"/5/on\"><button class=\"button2\">ON</button></a></div>\n</div>\n" << std::endl;
    }
    else
    {
        std::cout << "<div class=\"buttondiv\"><a href=\"/5/off\"><button class=\"button\">OFF</button></a></div>\n</div>\n" << std::endl;
    } 

    // Display current state, and ON/OFF buttons for GPIO 4  
    std::cout << "<div>\n<div class=\"titlediv\">GPIO 4 - State " + output4State + "</div>\n" << std::endl;

    // If the output4State is off, it displays the ON button       
    if (output4State=="off") 
    {
        std::cout << "<div class=\"buttondiv\"><a href=\"/4/on\"><button class=\"button2\">ON</button></a></div>\n</div>\n" << std::endl;
    }
    else
    {
        std::cout << "<div class=\"buttondiv\"><a href=\"/4/off\"><button class=\"button\">OFF</button></a></div>\n</div>\n" << std::endl;
    }
#endif

    std::cout << "</table>\n</body>\n</html>\n" << std::endl;

    // The HTTP response ends with another blank line
    std::cout << "\r\n\r\n" << std::endl;
}
#endif

//
// so we have 16 additional IO ports over the I2C bus with the MCP23017
// for now lets see if we can light up a few leds first to check outputs
// then we want to read a couple buttons to check inputs
// sounds fun and exciting for sure.  need to find my gvimrc file.  bah.
//
#if RUN_ON_ESP8266
void displayCurrentOutputs( WiFiClient & client )
{

}
#else
void displayCurrentOutputs( void )
{

}
#endif

#if RUN_ON_ESP8266
void writeHtmlPageHeaders( WiFiClient & client )
{
    // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
    // and a content-type so the client knows what's coming, then a blank line:
    //
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();
}
#else
void writeHtmlPageHeaders( void )
{
    // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
    // and a content-type so the client knows what's coming, then a blank line:
    //
    std::cout << "HTTP/1.1 200 OK" << std::endl;
    std::cout << "Content-type:text/html" << std::endl;
    std::cout << "Connection: close" << std::endl;
    std::cout << "\r\n\r\n" << std::endl;
}
#endif

#if RUN_ON_ESP8266
void writeHtmlPageCSS( WiFiClient & client )
{
    client.println("<!DOCTYPE html><html>");
    client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client.println("<link rel=\"icon\" href=\"data:,\">");
    client.println("<style> html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
    client.println(".button { background-color: #778899; color: white; padding: 2px 6px 2px 4px;");
    client.println("text-decoration: none; font-size: 14px; margin: 2px; cursor: pointer;}");
    client.println(".button2 { background-color: #90EE90; color: white; padding: 2px 6px 2px 4px;");
    client.println("text-decoration: none; font-size: 14px; margin: 2px; cursor: pointer;}");

    client.println("#outputs { font-family: Arial, Helvetica, sans-serif; border-collapse: collapse; width: auto; }");
    client.println("#outputs td, #outputs th { border: 1px solid #ddd; padding: 2px; }");
    client.println("#outputs td { text-align: center; }");
    client.println("#outputs tr:nth-child(odd) { background-color: #FAEBD7; }");
    client.println("#outputs tr:nth-child(even) { background-color: #F2F2F2; }");
    client.println("#outputs tr:hover { background-color: #DDD; }");
    client.println("#outputs th { padding-top: 12px; padding-bottom: 12px;  padding-left: 2px; padding-right: 2px; text-align: left; background-color: #04AA6D; color: white; }");

    client.println("</style></head>");
}
#else
void writeHtmlPageCSS( void )
{
    std::cout << "<!DOCTYPE html><html>" << std::endl;
    std::cout << "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl;
    std::cout << "<link rel=\"icon\" href=\"data:,\">" << std::endl;

    // CSS to style the on/off buttons 
    // Feel free to change the background-color and font-size attributes to fit your preferences
    //
    std::cout << "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}" << std::endl;
    std::cout << ".button { background-color: #778899; color: white; padding: 2px 6px 2px 4px;" << std::endl;
    std::cout << "text-decoration: none; font-size: 14px; margin: 2px; cursor: pointer;}" << std::endl;
    std::cout << ".button2 { background-color: #90EE90; color: white; padding: 2px 6px 2px 4px;" << std::endl;
    std::cout << "text-decoration: none; font-size: 14px; margin: 2px; cursor: pointer;}" << std::endl;
    std::cout << "div { padding: 4px 3px 2px 5px; width:100%; overflow: hidden; }" << std::endl;
    std::cout << ".titlediv { float: left; width: auto; height: 26px; background-color: #FAEBD7; text-align: center;}" << std::endl;
    std::cout << ".buttondiv { float: left; width: auto; height: 30px; background-color: #F0FFFF; vertical-align: center;}" << std::endl;

    std::cout << "#outputs { font-family: Arial, Helvetica, sans-serif; border-collapse: collapse; width: auto; }" << std::endl;
    std::cout << "#outputs td, #outputs th { border: 1px solid #ddd; padding: 2px; }" << std::endl;
    std::cout << "#outputs td { text-align: center; }" << std::endl;
    std::cout << "#outputs tr:nth-child(odd) { background-color: #FAEBD7; }" << std::endl;
    std::cout << "#outputs tr:nth-child(even) { background-color: #F2F2F2; }" << std::endl;
    std::cout << "#outputs tr:hover { background-color: #DDD; }" << std::endl;
    std::cout << "#outputs th { padding-top: 12px; padding-bottom: 12px;  padding-left: 2px; padding-right: 2px; text-align: left; background-color: #04AA6D; color: white; }" << std::endl;

    std::cout << "</style></head>" << std::endl;
}
#endif

//
// I2C bus scanning code taken from 
// https://forum.arduino.cc/t/solved-esp-01-i2c-mcp23017-and-the-arduino-ide/932889/11
// refactored into function that dumps to web page as scan takes place instead of Serial.println
//
// changed to break on the first I2C device found and return the address
//
// TODO: add a starting address so the scan can continue from whatever the last
// returned address or another starting address for additional capability
//
#if RUN_ON_ESP8266
byte scan_I2C_for_mcp23017(
        String & output,
        const bool printSerial )
{
    byte error = 0;
    byte address = 0;
    int nDevices = 0;
    output = "";

    if( printSerial ) Serial.println("Scanning...");
    output.concat("Scanning for I2C devices...\n");

    for( address = 1; (address < 127) && (nDevices < 1); address++ ) 
    {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
            if( printSerial ) Serial.print("I2C device found at address 0x");
            output.concat("I2C device found at address 0x");

            if (address<16) 
            {
                if( printSerial ) Serial.print("0");
                output.concat("0");
            }

            if( printSerial ) Serial.println(address,HEX);
            output.concat( String(address,HEX) );
            output.concat("\n");
            nDevices++;
        }
        else if (error==4) 
        {
            if( printSerial ) Serial.print("Unknow error at address 0x");
            output.concat("Unknow error at address 0x");

            if (address<16) 
            {
                if( printSerial ) Serial.print("0");
                output.concat("0");
            }

            if( printSerial ) Serial.println(address,HEX);
            output.concat( String(address,HEX) );
            output.concat("\n");
        }    
    }

    if (nDevices == 0) 
    {
        if( printSerial ) Serial.println("No I2C devices found\n");
        output.concat("No I2C devices found\n");
        address = 0;
    }
    else
    {
        if( printSerial ) Serial.println("done\n");
        output.concat("done\n");
    }

    return address;        
}
#else
int8_t scan_I2C_for_mcp23017(
        std::string & output,
        const bool printSerial )
{
    int8_t address = 0;
#if 0
    int8_t error = 0;
    int nDevices = 0;
    output = "";

    if( printSerial ) Serial.println("Scanning...");
    output.concat("Scanning for I2C devices...\n");

    for( address = 1; (address < 127) && (nDevices < 1); address++ ) 
    {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
            if( printSerial ) Serial.print("I2C device found at address 0x");
            output.concat("I2C device found at address 0x");

            if (address<16) 
            {
                if( printSerial ) Serial.print("0");
                output.concat("0");
            }

            if( printSerial ) Serial.println(address,HEX);
            output.concat( String(address,HEX) );
            output.concat("\n");
            nDevices++;
        }
        else if (error==4) 
        {
            if( printSerial ) Serial.print("Unknow error at address 0x");
            output.concat("Unknow error at address 0x");

            if (address<16) 
            {
                if( printSerial ) Serial.print("0");
                output.concat("0");
            }

            if( printSerial ) Serial.println(address,HEX);
            output.concat( String(address,HEX) );
            output.concat("\n");
        }    
    }

    if (nDevices == 0) 
    {
        if( printSerial ) Serial.println("No I2C devices found\n");
        output.concat("No I2C devices found\n");
        address = 0;
    }
    else
    {
        if( printSerial ) Serial.println("done\n");
        output.concat("done\n");
    }

#endif
    return address;        
}
#endif

//
// displays at startup the Sketch running in the Arduino
// https://forum.arduino.cc/t/solved-esp-01-i2c-mcp23017-and-the-arduino-ide/932889/11
// refactored into function that dumps to web page as scan takes place instead of Serial.println
//

#if RUN_ON_ESP8266
void display_Running_Sketch( String & output )
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
#else
void display_Running_Sketch( std::string & output )
{
    std::string the_path = __FILE__;
    std::string the_sketchname = the_path;
    size_t slash_loc = the_path.rfind('/');

    if( slash_loc != std::string::npos )
    {
        std::string the_cpp_name = the_path.substr(slash_loc+1);

        size_t dot_loc = the_cpp_name.rfind('.');

        if( dot_loc != std::string::npos )
        {
            the_sketchname = the_cpp_name.substr(0, dot_loc);
        }
    }

    output.append("\nArduino is running Sketch: ");
    output.append(the_sketchname);
    output.append("\nCompiled on: ");
    output.append(__DATE__);
    output.append(" at ");
    output.append(__TIME__);
    output.append("\n");
}
#endif
