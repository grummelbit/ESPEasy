#ifdef USES_C009
//#######################################################################################################
//########################### Controller Plugin 014: MQTT-Dooblr-InfluxDB ##########################################
//#######################################################################################################

/*******************************************************************************
 * Copyright 2018 grummelBit
 * Contact: derhurz@gmx.net
 *
 * Release notes:
 - v0.1
 /******************************************************************************/

#define CPLUGIN_014
#define CPLUGIN_ID_014         14
#define CPLUGIN_NAME_014       "MQTT-Dooblr-InfluxDB"
#include <ArduinoJson.h>

boolean CPlugin_014(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {
    case CPLUGIN_PROTOCOL_ADD:
      {
        Protocol[++protocolCount].Number = CPLUGIN_ID_014;
        Protocol[protocolCount].usesMQTT = true;
        Protocol[protocolCount].usesTemplate = true;
        Protocol[protocolCount].usesAccount = true;
        Protocol[protocolCount].usesPassword = true;
        Protocol[protocolCount].defaultPort = 1883;
        Protocol[protocolCount].usesID = false;
        break;
      }

      case CPLUGIN_GET_DEVICENAME:
        {
          string = F(CPLUGIN_NAME_014);
          break;
        }

      case CPLUGIN_PROTOCOL_TEMPLATE:
        {
          event->String1 = F("/home/office/%sysname%/#");
          event->String2 = F("/home/office/%sysname%/%tskname%");
          break;
        }

      // case CPLUGIN_PROTOCOL_RECV:
      //   {
      //     byte ControllerID = findFirstEnabledControllerWithId(CPLUGIN_ID_005);
      //     if (ControllerID == CONTROLLER_MAX) {
      //       // Controller is not enabled.
      //       break;
      //     } else {
      //       String cmd;
      //       struct EventStruct TempEvent;
      //       bool validTopic = false;
      //       const int lastindex = event->String1.lastIndexOf('/');
      //       const String lastPartTopic = event->String1.substring(lastindex + 1);
      //       if (lastPartTopic == F("cmd")) {
      //         cmd = event->String2;
      //         parseCommandString(&TempEvent, cmd);
      //         TempEvent.Source = VALUE_SOURCE_MQTT;
      //         validTopic = true;
      //       } else {
      //         if (lastindex > 0) {
      //           // Topic has at least one separator
      //           if (isFloat(event->String2) && isInt(lastPartTopic)) {
      //             int prevLastindex = event->String1.lastIndexOf('/', lastindex - 1);
      //             cmd = event->String1.substring(prevLastindex + 1, lastindex);
      //             TempEvent.Par1 = lastPartTopic.toInt();
      //             TempEvent.Par2 = event->String2.toFloat();
      //             TempEvent.Par3 = 0;
      //             validTopic = true;
      //           }
      //         }
      //       }
      //       if (validTopic) {
      //         // in case of event, store to buffer and return...
      //         String command = parseString(cmd, 1);
      //         if (command == F("event")) {
      //         eventBuffer = cmd.substring(6);
      //         } else if (!PluginCall(PLUGIN_WRITE, &TempEvent, cmd)) {
      //           remoteConfig(&TempEvent, cmd);
      //         }
      //       }
      //     }
      //     break;
      //   }

      case CPLUGIN_PROTOCOL_SEND:
        {
          if (!WiFiConnected(100)) {
            success = false;
            break;
          }
          ControllerSettingsStruct ControllerSettings;
          LoadControllerSettings(event->ControllerIndex, (byte*)&ControllerSettings, sizeof(ControllerSettings));

          statusLED(true);

          if (ExtraTaskSettings.TaskDeviceValueNames[0][0] == 0)
            PluginCall(PLUGIN_GET_DEVICEVALUENAMES, event, dummyString);

          String pubname = ControllerSettings.Publish;
          parseControllerVariables(pubname, event, false);

          // Create json root object
          DynamicJsonBuffer jsonBuffer;
          //JsonObject& root = jsonBuffer.createObject();
          JsonObject& val = jsonBuffer.createObject();
          //root[F("module")] = String(F("ESPEasy"));
          //root[F("version")] = String(F("1.04"));

          // Create nested objects
          //JsonObject& data = root.createNestedObject(String(F("data")));
          // JsonObject& ESP = data.createNestedObject(String(F("ESP")));
          // ESP[F("name")] = Settings.Name;
          // ESP[F("unit")] = Settings.Unit;
          // ESP[F("version")] = Settings.Version;
          // ESP[F("build")] = Settings.Build;
          // ESP[F("build_notes")] = BUILD_NOTES;
          // ESP[F("build_git")] = BUILD_GIT;
          // ESP[F("node_type_id")] = NODE_TYPE_ID;
          // ESP[F("sleep")] = Settings.deepSleep;

          // embed IP, important if there is NAT/PAT
          // char ipStr[20];
          // IPAddress ip = WiFi.localIP();
          // sprintf_P(ipStr, PSTR("%u.%u.%u.%u"), ip[0], ip[1], ip[2], ip[3]);
          // ESP[F("ip")] = WiFi.localIP().toString();

          // Create nested SENSOR json object
          // JsonObject& SENSOR = data.createNestedObject(String(F("SENSOR")));
          byte valueCount = getValueCountFromSensorType(event->sensorType);
          // char itemNames[valueCount][2];
          for (byte x = 0; x < valueCount; x++)
          {
            // Each sensor value get an own object (0..n)
            // sprintf(itemNames[x],"%d",x);
            // JsonObject& val = SENSOR.createNestedObject(String(x));

            // val[F("valueName")]  = ExtraTaskSettings.TaskDeviceValueNames[x];
            //val[F("type")]       = event->sensorType;
            // val[F("valueCount")] = (uint8)valueCount;

            // String tmppubname = pubname;
            // tmppubname.replace(F("%valname%"), ExtraTaskSettings.TaskDeviceValueNames[x]);

            if (event->sensorType == SENSOR_TYPE_LONG) {
              // val[F("value")] = (unsigned long)UserVar[event->BaseVarIndex] + ((unsigned long)UserVar[event->BaseVarIndex + 1] << 16);
              float fVal= (unsigned long)UserVar[event->BaseVarIndex] + ((unsigned long)UserVar[event->BaseVarIndex + 1] << 16);
              val[ExtraTaskSettings.TaskDeviceValueNames[x]] = fVal;
            }
            else { // All other sensor types

              float fVal = UserVar[event->BaseVarIndex + x];
              val[ExtraTaskSettings.TaskDeviceValueNames[x]] = fVal;
              // val[ExtraTaskSettings.TaskDeviceValueNames[x]] = floatToJson((float)UserVar[event->BaseVarIndex + x],2);
              // val[ExtraTaskSettings.TaskDeviceValueNames[x]] = formatUserVar(event, x);
            }

          // String value = "";
          // // byte DeviceIndex = getDeviceIndex(Settings.TaskDeviceNumber[event->TaskIndex]);
          // byte valueCount = getValueCountFromSensorType(event->sensorType);
          // for (byte x = 0; x < valueCount; x++)
          // {
          //   String tmppubname = pubname;
          //   tmppubname.replace(F("%valname%"), ExtraTaskSettings.TaskDeviceValueNames[x]);
          //   if (event->sensorType == SENSOR_TYPE_LONG)
          //     value = (unsigned long)UserVar[event->BaseVarIndex] + ((unsigned long)UserVar[event->BaseVarIndex + 1] << 16);
          //   else
          //     value = formatUserVar(event, x);


          }

          val[F("deviceName")] = ExtraTaskSettings.TaskDeviceName;
          // val[F("type")]       = event->sensorType;
          // val[F("valueCount")] = (uint8_t)valueCount;
          val[F("unit")] = Settings.Unit;

          // Lastly, you can print the resulting JSON to a String
          String value;
          // root.printTo(value);
          val.printTo(value);

            MQTTpublish(event->ControllerIndex, pubname.c_str(), value.c_str(), Settings.MQTTRetainFlag);
            String log = F("MQTT : ");
            log += pubname;
            log += " ";
            log += value.c_str();
            addLog(LOG_LEVEL_DEBUG, log);
          break;
        }
    }

    return success;
  }
  #endif
