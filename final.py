import sys
from linkkit import linkkit
import threading
import traceback
import inspect
import time
import logging
import serial

# config log
__log_format = '%(asctime)s-%(process)d-%(thread)d - %(name)s:%(module)s:%(funcName)s - %(levelname)s - %(message)s'
logging.basicConfig(format=__log_format)

com = serial.Serial("/dev/ttyS1", 115200, timeout=1)


class CustomerThing(object):
    def __init__(self):
        self.__linkkit = linkkit.LinkKit(
            host_name="cn-shanghai",
            product_key="j1igL9G0rK7",
            device_name="ESP8266_2",
            device_secret="5b622d5d19df37320fc945ee32509c49")
        self.__linkkit.enable_logger(logging.DEBUG)
        self.__linkkit.on_device_dynamic_register = self.on_device_dynamic_register
        self.__linkkit.on_connect = self.on_connect
        self.__linkkit.on_disconnect = self.on_disconnect
        self.__linkkit.on_topic_message = self.on_topic_message
        self.__linkkit.on_subscribe_topic = self.on_subscribe_topic
        self.__linkkit.on_unsubscribe_topic = self.on_unsubscribe_topic
        self.__linkkit.on_publish_topic = self.on_publish_topic
        self.__linkkit.on_thing_enable = self.on_thing_enable
        self.__linkkit.on_thing_disable = self.on_thing_disable
        self.__linkkit.on_thing_event_post = self.on_thing_event_post
        self.__linkkit.on_thing_prop_post = self.on_thing_prop_post
        self.__linkkit.on_thing_prop_changed = self.on_thing_prop_changed
        self.__linkkit.on_thing_call_service = self.on_thing_call_service
        self.__linkkit.on_thing_raw_data_post = self.on_thing_raw_data_post
        self.__linkkit.on_thing_raw_data_arrived = self.on_thing_raw_data_arrived
        self.__linkkit.thing_setup("tsl.json")
        self.__linkkit.config_device_info("Eth|03ACDEFF0032|Eth|03ACDEFF0031")
        self.__call_service_request_id = 0

    def on_device_dynamic_register(self, rc, value, userdata):
        if rc == 0:
            print("dynamic register device success, value:" + value)
        else:
            print("dynamic register device fail, message:" + value)

    def on_connect(self, session_flag, rc, userdata):
        print("on_connect:%d,rc:%d,userdata:" % (session_flag, rc))

    def on_disconnect(self, rc, userdata):
        print("on_disconnect:rc:%d,userdata:" % rc)

    def on_topic_message(self, topic, payload, qos, userdata):
        print("on_topic_message:" + topic + " payload:" + str(payload) + " qos:" + str(qos))
        pass

    def on_subscribe_topic(self, mid, granted_qos, userdata):
        print("on_subscribe_topic mid:%d, granted_qos:%s" %
              (mid, str(','.join('%s' % it for it in granted_qos))))
        pass

    def on_unsubscribe_topic(self, mid, userdata):
        print("on_unsubscribe_topic mid:%d" % mid)
        pass

    def on_publish_topic(self, mid, userdata):
        print("on_publish_topic mid:%d" % mid)

    def on_thing_prop_changed(self, params, userdata):
        print("on_thing_prop_changed params:" + str(params))

    def on_thing_enable(self, userdata):
        print("on_thing_enable")

    def on_thing_disable(self, userdata):
        print("on_thing_disable")

    def on_thing_event_post(self, event, request_id, code, data, message, userdata):
        print("on_thing_event_post event:%s,request id:%s, code:%d, data:%s, message:%s" %
              (event, request_id, code, str(data), message))
        pass

    def on_thing_prop_post(self, request_id, code, data, message,userdata):
        print("on_thing_prop_post request id:%s, code:%d, data:%s message:%s" %
              (request_id, code, str(data), message))

    def on_thing_raw_data_arrived(self, payload, userdata):
        print("on_thing_raw_data_arrived:%s" % str(payload))

    def on_thing_raw_data_post(self, payload, userdata):
        print("on_thing_raw_data_post: %s" % str(payload))

    def on_thing_call_service(self, identifier, request_id, params, userdata):
        print("on_thing_call_service identifier:%s, request id:%s, params:%s" %
              (identifier, request_id, params))
        self.__call_service_request_id = request_id
        pass

    def user_loop(self):
        self.__linkkit.connect_async()
        tips = "1: disconnect\n" +\
               "2 connect&loop\n" +\
               "3 subscribe topic\n" + \
               "4 unsubscribe topic\n" + \
               "5 public topic\n" +\
               ""
        while True:
            s = com.readline().decode("utf-8").strip()
            if s != "":
                if s[0] == "!":
                    s = s[1::].split()
                    temp = float(s[0])
                    humi = float(s[1])
                    co2 = int(s[2])
                    tvoc = int(s[3])
                    lux = int(s[4])
                    m_id = int(s[5])
                    prop_data = {}
                    if m_id == 1:
                        prop_data = {
                            "CurrentHumidity1": humi,
                            "CurrentTemperature1": temp,
                            "Lux1":lux,
                            "tvoc1":tvoc,
                            "co21":co2,
                        }

                    if m_id == 0:
                        prop_data = {
                            "CurrentHumidity": humi,
                            "CurrentTemperature": temp,
                            "Lux":lux,
                            "tvoc":tvoc,
                            "co2":co2,
                        }

                    if m_id == 2:
                        prop_data = {
                            "CurrentHumidity2": humi,
                            "CurrentTemperature2": temp,
                            "Lux2":lux,
                            "tvoc2":tvoc,
                            "co22":co2,
                        }

                    self.__linkkit.thing_post_property(prop_data)
                    print("sent to server:", humi, temp, m_id)


custom_thing = CustomerThing()
custom_thing.user_loop()
