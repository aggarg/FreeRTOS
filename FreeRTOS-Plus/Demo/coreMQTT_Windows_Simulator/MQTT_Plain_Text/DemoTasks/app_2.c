/* Standard includes. */
#include <string.h>
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Demo Specific configs. */
#include "demo_config.h"

/* MQTT library includes. */
#include "core_mqtt.h"

/* Transport interface implementation include header for plaintext connection. */
#include "transport_plaintext.h"

/* Defines. */
#define mqttexampleTOPIC           "/example/topic"
#define mqttexampleMESSAGE         "Hello World!"
#define mqttexampleDELAY_BETWEEN_PUBLISHES_TICKS          ( pdMS_TO_TICKS( 2000U ) )

#define commandPUBLISH              1

struct NetworkContext
{
    PlaintextTransportParams_t* pParams;
};

/* Static functions. */
static void prvMQTTPublish( MQTTContext_t* pxMQTTContext );

static void Publisher( void * params );
static void MQTTAgent( void * params );

QueueHandle_t commandQueue = NULL;
/*-----------------------------------------------------------*/

void start_app( MQTTContext_t * pMqttCtx )
{
    xQueueCreate( 10, sizeof( uint32_t ) );
     xTaskCreate( Publisher, "Pub1", configMINIMAL_STACK_SIZE, ( void * ) 500, tskIDLE_PRIORITY, NULL );
     xTaskCreate( Publisher, "Pub2", configMINIMAL_STACK_SIZE, ( void * ) 1000, tskIDLE_PRIORITY, NULL );
     xTaskCreate( Publisher, "Pub3", configMINIMAL_STACK_SIZE, ( void * ) 1500, tskIDLE_PRIORITY, NULL );
     xTaskCreate( MQTTAgent, "Agent", configMINIMAL_STACK_SIZE, ( void * ) pMqttCtx, tskIDLE_PRIORITY, NULL );
}
/*-----------------------------------------------------------*/

static void MQTTAgent( void * params )
{
    uint32_t command;
    MQTTContext_t * pMqttCtx = ( MQTTContext_t * ) params;

    for( ;; )
    {
        xQueueReceive( commandQueue, &( command ), portMAX_DELAY );

        if( command == commandPUBLISH )
        {
            prvMQTTPublish( pMqttCtx );
        }
    }
}
/*-----------------------------------------------------------*/

static void Publisher( void * params )
{
    uint32_t command = commandPUBLISH;
    uint32_t timePeriod = ( uint32_t ) params;

    for( ;; )
    {
        xQueueSend( commandQueue, &( command ), 0 );
        vTaskDelay( pdMS_TO_TICKS( timePeriod ) );
    }
}
/*-----------------------------------------------------------*/

static void prvMQTTPublish( MQTTContext_t * pxMQTTContext )
{
    MQTTStatus_t xResult;
    MQTTPublishInfo_t xMQTTPublishInfo;
    uint16_t usPublishPacketIdentifier;

    /* Some fields are not used by this demo so start with everything at 0. */
    ( void ) memset( ( void * ) &xMQTTPublishInfo, 0x00, sizeof( xMQTTPublishInfo ) );

    xMQTTPublishInfo.qos = MQTTQoS0;
    xMQTTPublishInfo.retain = false;
    xMQTTPublishInfo.pTopicName = mqttexampleTOPIC;
    xMQTTPublishInfo.topicNameLength = ( uint16_t ) strlen( mqttexampleTOPIC );
    xMQTTPublishInfo.pPayload = mqttexampleMESSAGE;
    xMQTTPublishInfo.payloadLength = strlen( mqttexampleMESSAGE );

    /* Get a unique packet id. */
    usPublishPacketIdentifier = MQTT_GetPacketId( pxMQTTContext );

    LogInfo( ( "Publishing to the MQTT topic %s.\r\n", mqttexampleTOPIC ) );
    /* Send PUBLISH packet. */
    xResult = MQTT_Publish( pxMQTTContext, &xMQTTPublishInfo, usPublishPacketIdentifier );
    configASSERT( xResult == MQTTSuccess );
}
/*-----------------------------------------------------------*/
