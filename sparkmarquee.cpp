#include "sparkmarquee.h"

#include "TelnetServer/telnet-server-lib.h"
#include "MCP23017.h"


// Define the pins we're going to call pinMode on

int led2 = D7; // This one is the built-in tiny one to the right of the USB jack

MCP23017 clocks;
MCP23017 power;
Telnet::Server tServer(1758);
CommandEngine::CommandLineEngine cleg;



// Called when the telnet server is ready to send you data

void processData(Telnet::Server * ts, String txt, int flags) {
    switch (flags) {
        case Telnet::Server::CBF_WelcomeMsg: //on first connect this gets called
            ts->println("Hello and welcome to The Spark Marquee");
            ts->forceCommandPrompt();
            break;
        case Telnet::Server::CBF_Error: //on overflow or error
            ts->print(txt);
            ts->forceCommandPrompt();
            break;
        case Telnet::Server::CBF_CmdPrompt: // when the command promt is ready
            ts->print(">>");
            break;
        default:
            // Process the command
            cleg.execute(txt, ts); // call the CommandLineEngine
            ts->forceCommandPrompt(); // force the command prompt
    }
}
// This routine runs only once upon reset

void CommandFunction_PrintMCP(Print * printer, CommandEngine::Tokens & tk, CommandEngine::CommandLineEngine::FunctionFlags ff) {
    if (ff == CommandEngine::CommandLineEngine::FF_HelpText) {
        printer->println("Help with Echo command:");
        printer->println("echo [value1] [value2] [value3] [value...]");
        printer->println("[value] = either a macro $ or a literal value");
        printer->println("Mostly for debugging the engine.");
        return;
    }
    for(int i = 0;i < 0x15;i++){
        printer->print("Clocks Reg ");
        printer->print(i);
        printer->print(" ");
        printer->println(clocks.readRegister(i));
        printer->print("Power Reg ");
        printer->print(i);
        printer->print(" ");
        printer->println(power.readRegister(i));
    }
}


void CommandFunction_echo(Print * printer, CommandEngine::Tokens & tk, CommandEngine::CommandLineEngine::FunctionFlags ff) {
    if (ff == CommandEngine::CommandLineEngine::FF_HelpText) {
        printer->println("Help with Echo command:");
        printer->println("echo [value1] [value2] [value3] [value...]");
        printer->println("[value] = either a macro $ or a literal value");
        printer->println("Mostly for debugging the engine.");
        return;
    }
    String arg;
    int c = 0;
    while (tk.next(arg)) {
        printer->print("Argument ");
        printer->print(c++);
        printer->print(": ");
        printer->println(arg);
    }
}

void CommandFunction_information(Print * printer, CommandEngine::Tokens & tk, CommandEngine::CommandLineEngine::FunctionFlags ff) {
    if (ff == CommandEngine::CommandLineEngine::FF_HelpText) {
        printer->println("Lists some useful information.");
        printer->println("No arguments needed for this function.");
        return;
    }
    // Get information like the IP address
    printer->print("IP Address: ");
    printer->println(WiFi.localIP());
    printer->print("SSID: ");
    printer->println(WiFi.SSID());
    printer->print("Uptime: ");
    printer->print(millis() / 1000);
    printer->println(" seconds");
}

void setup() {
    // Initialize D0 + D7 pin as output
    // It's important you do this here, inside the setup() function rather than outside it or in the loop function.
    pinMode(D2, OUTPUT);
    digitalWrite(D2, LOW);
    delay(50);
    digitalWrite(D2, HIGH); //Reset both MCPS
  //  delay(50);
    
    clocks.begin();
    power.begin(1);
    clocks.pinMode(0,OUTPUT);
    power.pinMode(0,OUTPUT);
    clocks.digitalWrite(0,HIGH);
    power.digitalWrite(0,HIGH);// Register macros
    // Register macros
    cleg.registerMacro("high", "1");
    cleg.registerMacro("low", "0");
    // Register custom functions with the engine
    cleg.registerFunction("echo", CommandFunction_echo);
    cleg.registerFunction("information", CommandFunction_information);
    cleg.registerFunction("McpReg", CommandFunction_PrintMCP);

    tServer.setProcessCallback(processData);

    tServer.connect();
}

// This routine gets called repeatedly, like once every 5-15 milliseconds.
// Spark firmware interleaves background CPU activity associated with WiFi + Cloud activity with your code. 
// Make sure none of your code delays or blocks for too long (like more than 5 seconds), or weird things can happen.

void loop() {
    tServer.poll();


}

