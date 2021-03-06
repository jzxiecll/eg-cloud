/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander/Ian Craggs - initial API and implementation and/or initial documentation
 *******************************************************************************/

#include "MQTTClient.h"

static void NewMessageData(MessageData* md, MQTTString* aTopicName, MQTTMessage* aMessgage) {
  md->topicName = aTopicName;
  md->message = aMessgage;
}


static int getNextPacketId(Client *c) {
  return c->next_packetid = (c->next_packetid == MAX_PACKET_ID) ? 1 : c->next_packetid + 1;
}


static int sendPacket(Client* c, int length, Timer* timer)
{
  int rc = FAILURE, 
    sent = 0;
    
  while (sent < length && !expired(timer))
  {
      rc = c->ipstack->mqttwrite(c->ipstack, &c->buf[sent], length, left_ms(timer));
      if (rc < 0)  // there was an error writing the data
		break;
      sent += rc;
  }
  if (sent == length)
  {
      countdown_ms(&c->ping_timer, c->keepAliveInterval*1000); // record the fact that we have successfully sent the packet    
      rc = MQTT_SUCCESS;
  }
  else
      rc = FAILURE;
  return rc;
}


void MQTTClient(Client* c, Network* network, unsigned int command_timeout_ms, unsigned char* buf, size_t buf_size, unsigned char* readbuf, size_t readbuf_size)
{
  int i;
  c->ipstack = network;
    
  for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
    c->messageHandlers[i].topicFilter = 0;
  c->command_timeout_ms = command_timeout_ms;
  c->buf = buf;
  c->buf_size = buf_size;
  c->readbuf = readbuf;
  c->readbuf_size = readbuf_size;
  c->isconnected = 0;
  c->ping_outstanding = 0;
  c->defaultMessageHandler = NULL;
  InitTimer(&c->ping_timer);
  InitTimer(&c->pingresp_timer);
  MutexInit(&c->mutex);
}


void MQTTClientDeinit(Client *c)
{
  if (!c) return;
  TimerDeinit(&c->ping_timer);
  TimerDeinit(&c->pingresp_timer);
  MutexDeinit(&c->mutex);
}

static int decodePacket(Client* c, int* value, int timeout)
{
  unsigned char i;
  int multiplier = 1;
  int len = 0;
  const int MAX_NO_OF_REMAINING_LENGTH_BYTES = 4;

  *value = 0;
  do
  {
      int rc = MQTTPACKET_READ_ERROR;

      if (++len > MAX_NO_OF_REMAINING_LENGTH_BYTES)
      {
		  rc = MQTTPACKET_READ_ERROR; /* bad data */
		  goto exit;
      }
      rc = c->ipstack->mqttread(c->ipstack, &i, 1, timeout);
      if (rc != 1)
		  goto exit;
      *value += (i & 127) * multiplier;
      multiplier *= 128;
   } while ((i & 128) != 0);
 exit:
  return len;
}


static int readPacket(Client* c, Timer* timer) 
{
  int rc = FAILURE;
  MQTTHeader header = {0};
  int len = 0;
  int rem_len = 0;

  /* 1. read the header byte.  This has the packet type in it */
  if (c->ipstack->mqttread(c->ipstack, c->readbuf, 1, left_ms(timer)) != 1)
    goto exit;

  len = 1;
  /* 2. read the remaining length.  This is variable in itself */
  decodePacket(c, &rem_len, left_ms(timer));
  len += MQTTPacket_encode(c->readbuf + 1, rem_len); /* put the original remaining length back into the buffer */

  /* 3. read the rest of the buffer using a callback to supply the rest of the data */
  if (rem_len > 0 && (c->ipstack->mqttread(c->ipstack, c->readbuf + len, rem_len, left_ms(timer)) != rem_len))
    goto exit;

  header.byte = c->readbuf[0];
  rc = header.bits.type;
 exit:
  return rc;
}


// assume topic filter and name is in correct format
// # can only be at end
// + and # can only be next to separator
static char isTopicMatched(char* topicFilter, MQTTString* topicName)
{
  char* curf = topicFilter;
  char* curn = topicName->lenstring.data;
  char* curn_end = curn + topicName->lenstring.len;
    
  while (*curf && curn < curn_end)
  {
      if (*curn == '/' && *curf != '/')
			break;
      if (*curf != '+' && *curf != '#' && *curf != *curn)
			break;
      if (*curf == '+')
      {   // skip until we meet the next separator, or end of string
	 		 char* nextpos = curn + 1;
	  		 while (nextpos < curn_end && *nextpos != '/')
	   		 nextpos = ++curn + 1;
      }
      else if (*curf == '#')
		curn = curn_end - 1;    // skip until end of string
      curf++;
      curn++;
  };
    
  return (curn == curn_end) && (*curf == '\0');
}


static int deliverMessage(Client* c, MQTTString* topicName, MQTTMessage* message)
{
  int i;
  int rc = FAILURE;
#if 0
  // we have to find the right message handler - indexed by topic
  for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
  {
      if (c->messageHandlers[i].topicFilter != 0 && (MQTTPacket_equals(topicName, (char*)c->messageHandlers[i].topicFilter) ||
						     isTopicMatched((char*)c->messageHandlers[i].topicFilter, topicName)))
        {
	  		if (c->messageHandlers[i].fp != NULL)
            {
		      MessageData md;
		      NewMessageData(&md, topicName, message);
		      c->messageHandlers[i].fp(&md);
		      rc = MQTT_SUCCESS;
            }
        }
  }
    
  if (rc == FAILURE && c->defaultMessageHandler != NULL) 
  {
      MessageData md;
      NewMessageData(&md, topicName, message);
      c->defaultMessageHandler(&md);
      rc = MQTT_SUCCESS;
  }   

#endif

  if (c->messageHandlers[0].fp != NULL)
  {
	  MessageData md;
	  NewMessageData(&md, topicName, message);
	  c->messageHandlers[0].fp(&md);
	  rc = MQTT_SUCCESS;
  }

  return rc;

}


static  int keepalive(Client* c)
{
  int rc = MQTT_SUCCESS;
  if (c->keepAliveInterval == 0)
  {
	  rc = MQTT_SUCCESS;
	  goto exit;
  }

  if (expired(&c->ping_timer))
  {
      if (!c->ping_outstanding)
      {
		  Timer timer;
		  InitTimer(&timer);
		  countdown_ms(&timer, 1000);
		  int len = MQTTSerialize_pingreq(c->buf, c->buf_size);
		  if (len > 0 && (rc = sendPacket(c, len, &timer)) == MQTT_SUCCESS) // send the ping packet
		  {
		      countdown_ms(&c->pingresp_timer, c->command_timeout_ms);
			  c->ping_outstanding = 1;
		      EG_DEBUG("send ping request.\r\n");
		      rc = MQTT_SUCCESS;
		  }
		  else 
		  {
		    EG_DEBUG("send ping failed.\r\n");
		    rc = FAILURE;
		  }	
      }
      c->ping_outstanding = 1;
    }
exit:
  return rc;
}


static int cycle(Client* c, Timer* timer)
{
  // read the socket, see what work is due
  unsigned short packet_type = readPacket(c, timer);
    
  int len = 0,rc = MQTT_SUCCESS;

  switch (packet_type)
  {
    case CONNACK:
    case PUBACK:
    case SUBACK:
     		 break;
    case PUBLISH:
    {
		MQTTString topicName;
		MQTTMessage msg;
		if (MQTTDeserialize_publish((unsigned char*)&msg.dup, (int*)&msg.qos, (unsigned char*)&msg.retained, (unsigned short*)&msg.id, &topicName,
					    (unsigned char**)&msg.payload, (int*)&msg.payloadlen, c->readbuf, c->readbuf_size) != 1)
		  	goto exit;
		deliverMessage(c, &topicName, &msg);
		if (msg.qos != QOS0)
		{
		    if (msg.qos == QOS1)
		      len = MQTTSerialize_ack(c->buf, c->buf_size, PUBACK, 0, msg.id);
		    else if (msg.qos == QOS2)
		      len = MQTTSerialize_ack(c->buf, c->buf_size, PUBREC, 0, msg.id);
		    if (len <= 0)
		      rc = FAILURE;
		    else {
		
		      Timer acktimer;
		      InitTimer(&acktimer);
		      countdown_ms(&acktimer, 20);
		      rc = sendPacket(c, len, &acktimer);
		    }

		    if (rc == FAILURE)
		      goto exit; // there was a problem
		}        
		break;
    }
    case PUBREC:
    {
		unsigned short mypacketid;
		unsigned char dup, type;
		if (MQTTDeserialize_ack(&type, &dup, &mypacketid, c->readbuf, c->readbuf_size) != 1)
		  rc = FAILURE;
		else if ((len = MQTTSerialize_ack(c->buf, c->buf_size, PUBREL, 0, mypacketid)) <= 0)
		  rc = FAILURE;
		else if ((rc = sendPacket(c, len, timer)) != MQTT_SUCCESS) // send the PUBREL packet
		  rc = FAILURE; // there was a problem
		if (rc == FAILURE)
		  goto exit; // there was a problem
		break;
    }
    case PUBCOMP:
      break;
    case PINGRESP:
      c->ping_outstanding = 0;
	  // countdown_ms(&c->ping_timer, c->keepAliveInterval*1000);
      break;
  }
  
  int kret = keepalive(c);
 // if (c->ping_outstanding && expired(&c->pingresp_timer)||(kret == -1))

  if (c->ping_outstanding && expired(&c->pingresp_timer))
  {
      //c->ping_outstanding = 0;
	  c->ping_outstanding = 0;
      rc = CONNECTION_LOST;
  }

 exit:
  if (rc == MQTT_SUCCESS)
    rc = packet_type;
  return rc;
}

int ret = MQTT_SUCCESS;

int MQTTYield(Client* c, int timeout_ms)
{
  volatile int rc, rc1 = MQTT_SUCCESS;
  Timer timer;

  InitTimer(&timer);    
  countdown_ms(&timer, timeout_ms);

  ret = MQTT_SUCCESS;

  while (!expired(&timer))
  {

      MutexLock(&c->mutex);

      rc1 = cycle(c, &timer);
      if (rc1 == FAILURE)
      {
	  	ret = FAILURE;
	  }else if (rc1 == CONNECTION_LOST)
      {
	    ret = CONNECTION_LOST;
	  }else {
			
      }
	  
      MutexUnlock(&c->mutex);
  }        
  return ret;
}


// only used in single-threaded mode where one command at a time is in process
static int waitfor(Client* c, int packet_type, Timer* timer)
{
  int rc = FAILURE;
    
  do
  {
    if (expired(timer)) 
		break; // we timed out
  }
  while ((rc = cycle(c, timer)) != packet_type);  
    
  return rc;
}


int MQTTConnect(Client* c, MQTTPacket_connectData* options)
{
  Timer connect_timer;
  int rc = FAILURE;
  MQTTPacket_connectData default_options = MQTTPacket_connectData_initializer;
  int len = 0;
    
  MutexLock(&c->mutex);

  InitTimer(&connect_timer);
  countdown_ms(&connect_timer, c->command_timeout_ms);

  if (c->isconnected) // don't send connect packet again if we are already connected
    goto exit;

  if (options == 0)
    options = &default_options; // set default options if none were supplied
    
  c->keepAliveInterval = options->keepAliveInterval;
  countdown_ms(&c->ping_timer, c->keepAliveInterval*1000);
  if ((len = MQTTSerialize_connect(c->buf, c->buf_size, options)) <= 0)
    goto exit;
  if ((rc = sendPacket(c, len, &connect_timer)) != MQTT_SUCCESS)
  {// send the connect packet

		 EG_DEBUG("========================sendPacket %d\r\n\r\n",left_ms(&connect_timer));
   		 goto exit; // there was a problem
  }
  // this will be a blocking call, wait for the connack
  if (waitfor(c, CONNACK, &connect_timer) == CONNACK)
  {
      unsigned char connack_rc = 255;
      char sessionPresent = 0;
      if (MQTTDeserialize_connack((unsigned char*)&sessionPresent, &connack_rc, c->readbuf, c->readbuf_size) == 1)
		rc = connack_rc;
      else
		rc = FAILURE;
  }
  else{
	  EG_DEBUG("***********************waitfor ACK timeout!\r\n \r\n");
	  rc = FAILURE;
  }
   
    
 exit:
  if (rc == MQTT_SUCCESS)
      c->isconnected = 1;

  MutexUnlock(&c->mutex);

  return rc;
}


int MQTTSubscribe(Client* c, const char* topicFilter, enum QoS qos, messageHandler messageHandler)
{ 
  int rc = FAILURE;  
  Timer timer;
  int len = 0;
  MQTTString topic = MQTTString_initializer;
  topic.cstring = (char *)topicFilter;

  MutexLock(&c->mutex);
    
  InitTimer(&timer);
  countdown_ms(&timer, c->command_timeout_ms);

  if (!c->isconnected)
    goto exit;
    
  len = MQTTSerialize_subscribe(c->buf, c->buf_size, 0, getNextPacketId(c), 1, &topic, (int*)&qos);
  if (len <= 0)
    goto exit;
  if ((rc = sendPacket(c, len, &timer)) != MQTT_SUCCESS) // send the subscribe packet
    goto exit;             // there was a problem
    
  if (waitfor(c, SUBACK, &timer) == SUBACK)      // wait for suback 
  {
      int count = 0, grantedQoS = -1;
      unsigned short mypacketid;
      if (MQTTDeserialize_suback(&mypacketid, 1, &count, &grantedQoS, c->readbuf, c->readbuf_size) == 1)
			rc = grantedQoS; // 0, 1, 2 or 0x80 
      if (rc != 0x80)
      {
		  int i;
		  for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
	      {
		      if (c->messageHandlers[i].topicFilter == 0)
	          {
				  c->messageHandlers[i].topicFilter = topicFilter;
				  c->messageHandlers[i].fp = messageHandler;
				  rc = 0;
				  break;
	          }
	     }
      }
  }
  else 
    rc = FAILURE;
        
 exit:

  MutexUnlock(&c->mutex);
  return rc;
}


int MQTTUnsubscribe(Client* c, const char* topicFilter)
{   
  int rc = FAILURE;
  Timer timer;    
  MQTTString topic = MQTTString_initializer;
  topic.cstring = (char *)topicFilter;
  int len = 0;

  MutexLock(&c->mutex);

  InitTimer(&timer);
  countdown_ms(&timer, c->command_timeout_ms);
    
  if (!c->isconnected)
    goto exit;
    
  if ((len = MQTTSerialize_unsubscribe(c->buf, c->buf_size, 0, getNextPacketId(c), 1, &topic)) <= 0)
    goto exit;
  if ((rc = sendPacket(c, len, &timer)) != MQTT_SUCCESS) // send the subscribe packet
    goto exit; // there was a problem
    
  if (waitfor(c, UNSUBACK, &timer) == UNSUBACK)
  {
      unsigned short mypacketid;  // should be the same as the packetid above
      if (MQTTDeserialize_unsuback(&mypacketid, c->readbuf, c->readbuf_size) == 1)
		rc = 0; 
  }
  else
    	rc = FAILURE;
    
 exit:

  MutexUnlock(&c->mutex);
  return rc;
}


int MQTTPublish(Client* c, const char* topicName, MQTTMessage* message)
{
  int rc = FAILURE;
  Timer timer;   
  MQTTString topic = MQTTString_initializer;
  topic.cstring = (char *)topicName;
  int len = 0;

  MutexLock(&c->mutex);

  InitTimer(&timer);
  countdown_ms(&timer, c->command_timeout_ms);
    
  if (!c->isconnected)
    goto exit;

  if (message->qos == QOS1 || message->qos == QOS2)
    message->id = getNextPacketId(c);
    
  len = MQTTSerialize_publish(c->buf, c->buf_size, 0, message->qos, message->retained, message->id, 
			      topic, (unsigned char*)message->payload, message->payloadlen);
  if (len <= 0)
    goto exit;
  if ((rc = sendPacket(c, len, &timer)) != MQTT_SUCCESS) 
  {  // send the subscribe packet   
    EG_DEBUG("sendPacket failed.\r\n");
    goto exit; // there was a problem
  }

  if (message->qos == QOS1)
  {
      if (waitfor(c, PUBACK, &timer) == PUBACK)
      {
			  unsigned short mypacketid;
			  unsigned char dup, type;
			  if (MQTTDeserialize_ack(&type, &dup, &mypacketid, c->readbuf, c->readbuf_size) != 1)
			   			 rc = FAILURE;
      }
      else 
	  {
			EG_DEBUG(" wait for puback timeout.\r\n");
			rc = FAILURE;
      }
  }
  else if (message->qos == QOS2)
  {
      if (waitfor(c, PUBCOMP, &timer) == PUBCOMP)
      {
		  unsigned short mypacketid;
		  unsigned char dup, type;
		  if (MQTTDeserialize_ack(&type, &dup, &mypacketid, c->readbuf, c->readbuf_size) != 1)
		    rc = FAILURE;
      }
      else
			rc = FAILURE;
  }
    
 exit:

  MutexUnlock(&c->mutex);

  return rc;
}


int MQTTDisconnect(Client* c)
{  
  int rc = FAILURE;
  Timer timer;     // we might wait for incomplete incoming publishes to complete

  MutexLock(&c->mutex);

  int len = MQTTSerialize_disconnect(c->buf, c->buf_size);
  InitTimer(&timer);
  countdown_ms(&timer, c->command_timeout_ms);
  if (len > 0)
    rc = sendPacket(c, len, &timer);            // send the disconnect packet
  c->isconnected = 0;

  MutexUnlock(&c->mutex);

  return rc;
}

