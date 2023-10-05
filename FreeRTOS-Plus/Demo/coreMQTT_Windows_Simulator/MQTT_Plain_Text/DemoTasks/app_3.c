/* Standard includes. */
#include <string.h>
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

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

static void Publisher1( void * params );
static void Publisher2( void * params );
static void Publisher3( void * params );

static SemaphoreHandle_t xSerializationMutex;
/*-----------------------------------------------------------*/

void start_app( MQTTContext_t * pMqttCtx )
{
    xSerializationMutex = xSemaphoreCreateMutex();
    xTaskCreate( Publisher1, "Pub1", configMINIMAL_STACK_SIZE, ( void * ) pMqttCtx, tskIDLE_PRIORITY, NULL );
    xTaskCreate( Publisher2, "Pub2", configMINIMAL_STACK_SIZE, ( void * ) pMqttCtx, tskIDLE_PRIORITY, NULL );
    xTaskCreate( Publisher3, "Pub3", configMINIMAL_STACK_SIZE, ( void * ) pMqttCtx, tskIDLE_PRIORITY, NULL );
}
/*-----------------------------------------------------------*/

static void Publisher1( void * params )
{
    MQTTContext_t * pxMQTTContext = ( MQTTContext_t * ) params;

    for( ;; )
    {
        xSemaphoreTake( xSerializationMutex, portMAX_DELAY );
        {
            prvMQTTPublish( pxMQTTContext );
        }
        xSemaphoreGive( xSerializationMutex );

        vTaskDelay( pdMS_TO_TICKS( 500 ) );
    }
}
/*-----------------------------------------------------------*/

static void Publisher2( void * params )
{
    MQTTContext_t * pxMQTTContext = ( MQTTContext_t * ) params;

    for( ;; )
    {
        xSemaphoreTake( xSerializationMutex, portMAX_DELAY );
        {
            prvMQTTPublish( pxMQTTContext );
        }
        xSemaphoreGive( xSerializationMutex );

        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
}
/*-----------------------------------------------------------*/

static void Publisher3( void * params )
{
    MQTTContext_t * pxMQTTContext = ( MQTTContext_t * ) params;

    for( ;; )
    {
        xSemaphoreTake( xSerializationMutex, portMAX_DELAY );
        {
            prvMQTTPublish( pxMQTTContext );
        }
        xSemaphoreGive( xSerializationMutex );

        vTaskDelay( pdMS_TO_TICKS( 1500 ) );
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
