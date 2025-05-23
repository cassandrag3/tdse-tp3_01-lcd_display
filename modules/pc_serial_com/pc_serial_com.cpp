//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "pc_serial_com.h"


#include "code.h"
#include "date_and_time.h"


//=====[Declaration of private defines]========================================


//=====[Declaration of private data types]=====================================

typedef enum{
    PC_SERIAL_COMMANDS,
    PC_SERIAL_GET_CODE,
    PC_SERIAL_SAVE_NEW_CODE,
} pcSerialComMode_t;

//=====[Declaration and initialization of public global objects]===============

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

char codeSequenceFromPcSerialCom[CODE_NUMBER_OF_KEYS];

//=====[Declaration and initialization of private global variables]============

static pcSerialComMode_t pcSerialComMode = PC_SERIAL_COMMANDS;
static bool codeComplete = false;
static int numberOfCodeChars = 0;

//=====[Declarations (prototypes) of private functions]========================

static void pcSerialComStringRead( char* str, int strLength );

static void pcSerialComGetCodeUpdate( char receivedChar );
static void pcSerialComSaveNewCodeUpdate( char receivedChar );

static void pcSerialComCommandUpdate( char receivedChar );

static void availableCommands();

//static void commandEnterCodeSequence();
static void commandEnterNewCode();

static void commandSetDateAndTime();
static void commandShowDateAndTime();


//=====[Implementations of public functions]===================================

void pcSerialComInit()
{
    availableCommands();
}

char pcSerialComCharRead()
{
    char receivedChar = '\0';
    if( uartUsb.readable() ) {
        uartUsb.read( &receivedChar, 1 );
    }
    return receivedChar;
}

void pcSerialComStringWrite( const char* str )
{
    uartUsb.write( str, strlen(str) );
}

void pcSerialComUpdate()
{
    char receivedChar = pcSerialComCharRead();
    if( receivedChar != '\0' ) {
        switch ( pcSerialComMode ) {
            case PC_SERIAL_COMMANDS:
                pcSerialComCommandUpdate( receivedChar );
            break;

            case PC_SERIAL_GET_CODE:
                pcSerialComGetCodeUpdate( receivedChar );
            break;

            case PC_SERIAL_SAVE_NEW_CODE:
                pcSerialComSaveNewCodeUpdate( receivedChar );
            break;
            default:
                pcSerialComMode = PC_SERIAL_COMMANDS;
            break;
        }
    }    
}

bool pcSerialComCodeCompleteRead()
{
    return codeComplete;
}

void pcSerialComCodeCompleteWrite( bool state )
{
    codeComplete = state;
}

//=====[Implementations of private functions]==================================

static void pcSerialComStringRead( char* str, int strLength )
{
    int strIndex;
    for ( strIndex = 0; strIndex < strLength; strIndex++) {
        uartUsb.read( &str[strIndex] , 1 );
        uartUsb.write( &str[strIndex] ,1 );
    }
    str[strLength]='\0';
}

static void pcSerialComGetCodeUpdate( char receivedChar )
{
    codeSequenceFromPcSerialCom[numberOfCodeChars] = receivedChar;
    pcSerialComStringWrite( "*" );
    numberOfCodeChars++;
   if ( numberOfCodeChars >= CODE_NUMBER_OF_KEYS ) {
        pcSerialComMode = PC_SERIAL_COMMANDS;
        codeComplete = true;
        numberOfCodeChars = 0;
    } 
}

static void pcSerialComSaveNewCodeUpdate( char receivedChar )
{
    static char newCodeSequence[CODE_NUMBER_OF_KEYS];

    newCodeSequence[numberOfCodeChars] = receivedChar;
    pcSerialComStringWrite( "*" );
    numberOfCodeChars++;
    if ( numberOfCodeChars >= CODE_NUMBER_OF_KEYS ) {
        pcSerialComMode = PC_SERIAL_COMMANDS;
        numberOfCodeChars = 0;
        codeWrite( newCodeSequence );
        pcSerialComStringWrite( "\r\nNew code configured\r\n\r\n" );
    } 
}

static void pcSerialComCommandUpdate( char receivedChar )
{
    switch (receivedChar) {

        //case '4': commandEnterCodeSequence(); break;
        case '5': commandEnterNewCode(); break;

        case 's': case 'S': commandSetDateAndTime(); break;
        case 't': case 'T': commandShowDateAndTime(); break;

        default: availableCommands(); break;
    } 
}

static void availableCommands()
{
    pcSerialComStringWrite( "Available commands:\r\n" );
    pcSerialComStringWrite( "Press '1' to get the alarm state\r\n" );
    pcSerialComStringWrite( "Press '2' to get the gas detector state\r\n" );
    pcSerialComStringWrite( "Press '3' to get the over temperature detector state\r\n" );
    pcSerialComStringWrite( "Press '4' to enter the code to deactivate the alarm\r\n" );
    pcSerialComStringWrite( "Press '5' to enter a new code to deactivate the alarm\r\n" );
    pcSerialComStringWrite( "Press 'f' or 'F' to get lm35 reading in Fahrenheit\r\n" );
    pcSerialComStringWrite( "Press 'c' or 'C' to get lm35 reading in Celsius\r\n" );
    pcSerialComStringWrite( "Press 's' or 'S' to set the date and time\r\n" );
    pcSerialComStringWrite( "Press 't' or 'T' to get the date and time\r\n" );
    pcSerialComStringWrite( "Press 'e' or 'E' to get the stored events\r\n" );
    pcSerialComStringWrite( "\r\n" );
}



static void commandEnterNewCode()
{
    pcSerialComStringWrite( "Please enter the new four digits numeric code " );
    pcSerialComStringWrite( "to deactivate the alarm: " );
    numberOfCodeChars = 0;
    pcSerialComMode = PC_SERIAL_SAVE_NEW_CODE;

}


static void commandSetDateAndTime()
{
    char year[5] = "";
    char month[3] = "";
    char day[3] = "";
    char hour[3] = "";
    char minute[3] = "";
    char second[3] = "";
    
    pcSerialComStringWrite("\r\nType four digits for the current year (YYYY): ");
    pcSerialComStringRead( year, 4);
    pcSerialComStringWrite("\r\n");

    pcSerialComStringWrite("Type two digits for the current month (01-12): ");
    pcSerialComStringRead( month, 2);
    pcSerialComStringWrite("\r\n");

    pcSerialComStringWrite("Type two digits for the current day (01-31): ");
    pcSerialComStringRead( day, 2);
    pcSerialComStringWrite("\r\n");

    pcSerialComStringWrite("Type two digits for the current hour (00-23): ");
    pcSerialComStringRead( hour, 2);
    pcSerialComStringWrite("\r\n");

    pcSerialComStringWrite("Type two digits for the current minutes (00-59): ");
    pcSerialComStringRead( minute, 2);
    pcSerialComStringWrite("\r\n");

    pcSerialComStringWrite("Type two digits for the current seconds (00-59): ");
    pcSerialComStringRead( second, 2);
    pcSerialComStringWrite("\r\n");
    
    pcSerialComStringWrite("Date and time has been set\r\n");

    dateAndTimeWrite( atoi(year), atoi(month), atoi(day), 
        atoi(hour), atoi(minute), atoi(second) );
}

static void commandShowDateAndTime()
{
    char str[100] = "";
    sprintf ( str, "Date and Time = %s", dateAndTimeRead() );
    pcSerialComStringWrite( str );
    pcSerialComStringWrite("\r\n");
}

