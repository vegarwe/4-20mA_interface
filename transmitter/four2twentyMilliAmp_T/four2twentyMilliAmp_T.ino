#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <SPI.h>
#include <Update.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WiFiUdp.h>


// Arduino UNO with Mikroe Arduino Uno Click shield
// 4-20mA is placed in socket #2
// CS   is pin 9
// SCK  is pin 13
// MISO is pin 12
// MOSI is pin 11
#define DAC_CS 21

const char* host = "esp32";
const char *ssid = "MyyourAP";
//const char *password = "yourPassword";

WebServer server(80);

const char* rootIndex =
  "<html>\n"
  "  <header>\n"
  "    <style>\n"
  //"      .slidecontainer {\n"
  //"        width: 850px; /*100%;*/ 100%; /* Width of the outside container */\n"
  //"      }\n"
  //"      \n"
  //"      /* The slider itself */\n"
  //"      .slider {\n"
  //"        -webkit-appearance: none;  /* Override default CSS styles */\n"
  //"        appearance: none;\n"
  //"        width: 800px; /*100%;*/ /* Full-width */\n"
  //"        height: 50px; /* Specified height */\n"
  //"        background: #d3d3d3; /* Grey background */\n"
  //"        outline: none; /* Remove outline */\n"
  //"        opacity: 0.7; /* Set transparency (for mouse-over effects on hover) */\n"
  //"        -webkit-transition: .2s; /* 0.2 seconds transition on hover */\n"
  //"        transition: opacity .2s;\n"
  //"      }\n"
  //"      \n"
  //"      /* Mouse-over effects */\n"
  //"      .slider:hover {\n"
  //"        opacity: 1; /* Fully shown on mouse-over */\n"
  //"      }\n"
  //"      \n"
  //"      /* The slider handle (use -webkit- (Chrome, Opera, Safari, Edge) and -moz- (Firefox) to override default look) */\n"
  //"      .slider::-webkit-slider-thumb {\n"
  //"        -webkit-appearance: none; /* Override default look */\n"
  //"        appearance: none;\n"
  //"        width: 20px; /* Set a specific slider handle width */\n"
  //"        height: 50px; /* Slider handle height */\n"
  //"        background: #04AA6D; /* Green background */\n"
  //"        cursor: pointer; /* Cursor on hover */\n"
  //"      }\n"
  //"      \n"
  //"      .slider::-moz-range-thumb {\n"
  //"        width: 20px; /* Set a specific slider handle width */\n"
  //"        height: 50px; /* Slider handle height */\n"
  //"        background: #04AA6D; /* Green background */\n"
  //"        cursor: pointer; /* Cursor on hover */\n"
  //"      }\n"
  "    </style>\n"
  "  </header>\n"
  "  <body>\n"
  "    <p><a href='/serverIndex'>update</a></p>\n"
  "    <div class='slidecontainer'>\n"
  "      <input type='range' min='320' max='2000' value='550' class='slider' id='myRange'>\n"
  "      <span>Value:</span> <span id='demo'></span>\n"
  "    </div>\n"
  "    <script type='text/javascript'>\n"
  "      //<![CDATA[\n"
  "      var slider = document.getElementById('myRange');\n"
  "      var output = document.getElementById('demo');\n"
  "      console.log(slider);\n"
  "      console.log(output);\n"
  "      output.innerHTML = slider.value; // Display the default slider value\n"
  "      \n"
  "      // Update the current slider value (each time you drag the slider handle)\n"
  "      slider.onchange = function() {\n"
  "        console.log(\"onchange \" + this.value);\n"
  "        output.innerHTML = this.value;\n"
  "        fetch(\"/current?value=\" + this.value);\n"
  "      }\n"
  "      //]]>\n"
  "    </script>\n"
  "  </body>\n"
  "</html>\n";

const char* serverIndex =
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>progress: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')"
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  SPI.begin();
  pinMode(DAC_CS, OUTPUT);

  set_DAC(320);

  WiFi.mode(WIFI_AP); //WiFi.mode(WIFI_STA);
  WiFi.softAP(ssid); //, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  Serial.println(ssid);


  //if (!MDNS.begin(host)) { //http://esp32.local
  //  Serial.println("Error setting up MDNS responder!");
  //  while (1) {
  //    delay(1000);
  //  }
  //}
  //Serial.println("mDNS responder started");

  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", rootIndex);
    Serial.println("Fisk");
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  server.on("/current", HTTP_GET, []() {
    //"/current?value=559"
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", "OK");
    if (server.args() > 0) {
      String path = server.arg(0);
      Serial.println("current");
      Serial.println(path);

      int received = constrain(path.toInt(), 0, 4095);

      Serial.print("DAC is set to value : ");
      Serial.println(received);

      set_DAC(received);
    }
  });
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
}

void loop()
{
  server.handleClient();

  while (Serial.available() > 0) {
    int received = Serial.parseInt();
    if (Serial.read() == '\n') {
      // constrain the values to 0 - 255 and invert
      received = constrain(received, 0, 4095);

      Serial.print("DAC is set to value : ");
      Serial.println(received);

      set_DAC(received);
    }
  }
}

void set_DAC(int set_value){
  byte first_byte;
  byte second_byte;

  first_byte = (set_value >> 8) & 0x0F;
  first_byte = first_byte | 0x30;
  second_byte = set_value & 0xFF;

  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(DAC_CS, 0);
  SPI.transfer(first_byte);
  SPI.transfer(second_byte);
  digitalWrite(DAC_CS, 1);
  SPI.endTransaction();
}
