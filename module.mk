
EGLINK_SRC = project/mt7687_hdk/apps/eg_cloud

C_FILES  += 	$(EGLINK_SRC)/src/eg_log.c
C_FILES  += 	$(EGLINK_SRC)/src/eg_cloud.c
C_FILES  += 	$(EGLINK_SRC)/src/eg_mqtt.c
C_FILES  += 	$(EGLINK_SRC)/src/eg_packet.c
C_FILES  += 	$(EGLINK_SRC)/src/eg_list.c
C_FILES  += 	$(EGLINK_SRC)/src/eg_smartlink.c
C_FILES  += 	$(EGLINK_SRC)/src/eg_porting_layer.c
C_FILES  += 	$(EGLINK_SRC)/src/eg_time.c
C_FILES  += 	$(EGLINK_SRC)/src/eg_aes.c
C_FILES  += 	$(EGLINK_SRC)/src/eg_hardware.c
C_FILES  += 	$(EGLINK_SRC)/src/eg_event.c

C_FILES      += $(EGLINK_SRC)/src/mqtt/eg_port_mqtt.c
C_FILES      += $(EGLINK_SRC)/src/mqtt/MQTTClient.c
C_FILES      += $(EGLINK_SRC)/src/mqtt/MQTTConnectClient.c
C_FILES      += $(EGLINK_SRC)/src/mqtt/MQTTConnectServer.c
C_FILES      += $(EGLINK_SRC)/src/mqtt/MQTTDeserializePublish.c
C_FILES      += $(EGLINK_SRC)/src/mqtt/MQTTFormat.c
C_FILES      += $(EGLINK_SRC)/src/mqtt/MQTTPacket.c
C_FILES      += $(EGLINK_SRC)/src/mqtt/MQTTSerializePublish.c
C_FILES      += $(EGLINK_SRC)/src/mqtt/MQTTSubscribeClient.c
C_FILES      += $(EGLINK_SRC)/src/mqtt/MQTTSubscribeServer.c
C_FILES      += $(EGLINK_SRC)/src/mqtt/MQTTUnsubscribeClient.c
C_FILES      += $(EGLINK_SRC)/src/mqtt/MQTTUnsubscribeServer.c

#################################################################################
#include path
CFLAGS 	+= -I$(SOURCE_DIR)/$(EGLINK_SRC)/inc
CFLAGS 	+= -I$(SOURCE_DIR)/$(EGLINK_SRC)/src/mqtt
CFLAGS	+= -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include/lwip
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/ports/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt$(PRODUCT_VERSION)/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
