#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

/*Broker Setup, using mosquitto in this projects*/
#define ADDRESS "tcp://169.254.1.172:1883"
#define CLIENTID "ExampleClientPub"
#define TOPIC "DJ415Y4H"
#define QOS 1
#define TIMEOUT 10000L
int fd;/*File Descriptor*/

void openport(void)
{
  /* ------------------------------- Opening the
    Serial Port ------------------------------- */
  /* Change /dev/ttyUSB0 to the one corresponding
    to your system */
  fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
  /* ttyUSB0 is the FT232 based USB2SERIAL Converter */
  /* O_RDWR - Read/Write access to serial port */
  /* O_NOCTTY - No terminal will control the process */
  /* Open in blocking mode,read will wait */
  /* ---------- Setting the Attributes of the
    serial port using termios structure ---------
  */
  struct termios SerialPortSettings; /*Create the
  structure*/
  tcgetattr(fd, &SerialPortSettings); /* Get the
  current attributes of the Serial port */
  /* Setting the Baud rate */
  cfsetispeed(&SerialPortSettings, B9600); /* Set
  Read Speed as 9600 */
  cfsetospeed(&SerialPortSettings, B9600); /* Set
  Write Speed as 9600 */
  /* 8N1 Mode */
  SerialPortSettings.c_cflag &= ~PARENB; /*
  Disables the Parity Enable bit(PARENB),So No
  Parity */
  SerialPortSettings.c_cflag &= ~CSTOPB; /* CSTOPB
  = 2 Stop bits,here it is cleared so 1 Stop
  bit */
  SerialPortSettings.c_cflag &= ~CSIZE; /* Clears
  the mask for setting the data size */
  SerialPortSettings.c_cflag |= CS8; /* Set the
  data bits = 8 */
  SerialPortSettings.c_cflag &= ~CRTSCTS; /* No
  Hardware flow Control */
  SerialPortSettings.c_cflag |= CREAD | CLOCAL; /*
  Enable receiver,Ignore Modem Control lines */
  SerialPortSettings.c_iflag &= ~(IXON | IXOFF |
                                  IXANY); /* Disable XON/XOFF flow control both
  i/p and o/p */
  SerialPortSettings.c_iflag &= ~(ICANON | ECHO |
                                  ECHOE | ISIG); /* Non Cannonical mode */
  SerialPortSettings.c_oflag &= ~OPOST; /* No
  Output Processing */
  /* Setting Time outs */
  SerialPortSettings.c_cc[VMIN] = 4; /* Read at
  least 10 characters */
  SerialPortSettings.c_cc[VTIME] = 0; /* Wait
  indefinetly */
  (tcsetattr(fd, TCSANOW, &SerialPortSettings));
}

int main(int argc, char* argv[])
{
  /*Initialization Setup*/
  MQTTClient client;
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  MQTTClient_message pubmsg = MQTTClient_message_initializer;
  MQTTClient_deliveryToken token;
  int rc;
  char read_buffer[32]; /* Buffer to store the data received*/
  int readBufferlen;
  while (1)
  {
    /* FLushing Old Data then read data from opening new port*/
    tcflush(fd, TCIFLUSH); /* Discards old data in the rx
  buffer */
    openport(); /* Opening Serial Port */
    read(fd, &read_buffer, 32); /* Read the data */
    /*Making MQTT Client*/
    MQTTClient_create(&client, ADDRESS, CLIENTID,
                      MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    /*Starting Connecting To Broker*/
    if ((rc = MQTTClient_connect(client, &conn_opts)) !=
        MQTTCLIENT_SUCCESS)
    {
      printf("Failed to connect, return code %d\n", rc);
      exit(-1);
    }
    /*Publishing The Message*/
    pubmsg.payload = read_buffer;
    pubmsg.payloadlen = strlen(read_buffer);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
    /*Echo data to screen*/
    printf("Publishing : %s\n", read_buffer);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", token);
  
    /*Terminating Client*/
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
  }
}

