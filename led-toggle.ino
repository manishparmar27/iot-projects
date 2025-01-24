#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Replace with your network credentials
const char* ssid = "realme 12 5G";       // Your Wi-Fi SSID
const char* password = "88888887"; // Your Wi-Fi Password

// Create a web server object on port 80
ESP8266WebServer server(8080);

// Define pin for LED
const int ledPin = 2; // GPIO5 (NodeMCU D1)
bool ledState = false; // LED state

// HTML content for the website
const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP8266 Dashboard</title>
  <link href="https://cdn.jsdelivr.net/npm/tailwindcss@2.2.19/dist/tailwind.min.css" rel="stylesheet">
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta3/css/all.min.css">
  <style>
    #loading {
      display: none; /* Hidden by default */
    }
  </style>
</head>
<body class="bg-gradient-to-r from-green-400 to-blue-400 text-center">
  <header class="bg-gray-800 text-white py-4 text-2xl shadow-md">
    ESP8266 Interactive Dashboard
  </header>
  
  <div id="loading" class="absolute inset-0 flex items-center justify-center bg-gray-800 bg-opacity-50">
    <div class="loader text-white">
      <i class="fas fa-spinner fa-spin fa-3x"></i>
      <p class="mt-2">Loading...</p>
    </div>
  </div>

  <div class="container mx-auto mt-10 p-6 max-w-md bg-white rounded-lg shadow-lg">
    <h2 class="text-xl font-bold text-gray-800 mb-4">Control the LED</h2>
    <p class="text-lg text-gray-600 mb-6">LED Status: <span id="ledStatus" class="font-semibold">%LED_STATE%</span></p>
    <div class="flex justify-center space-x-4">
      <button class="flex items-center justify-center px-4 py-2 bg-green-500 text-white rounded-lg shadow hover:bg-green-600 transition-transform transform hover:scale-105" onclick="toggleLED('on')" aria-label="Turn LED on">
        <i class="fas fa-lightbulb mr-2"></i> Turn ON
      </button>
      <button class="flex items-center justify-center px-4 py-2 bg-red-500 text-white rounded-lg shadow hover:bg-red-600 transition-transform transform hover:scale-105" onclick="toggleLED('off')" aria-label="Turn LED off">
        <i class="fas fa-lightbulb-slash mr-2"></i> Turn OFF
      </button>
    </div>
  </div>

  <script>
    // Function to fetch the current LED state on page load
    async function fetchLEDState() {
      showSpinner(); // Show loading animation
      try {
        const response = await fetch('/state'); // Replace with your actual endpoint
        const data = await response.text();
        document.getElementById('ledStatus').innerText = data;
      } catch (err) {
        console.error('Error fetching LED state:', err);
      } finally {
        hideSpinner(); // Hide loading animation
      }
    }

    async function toggleLED(state) {
      showSpinner(); // Show loading animation
      try {
        const response = await fetch(/${state}); // Replace with your actual endpoint
        const data = await response.text();
        document.getElementById('ledStatus').innerText = data;
      } catch (err) {
        console.error('Error toggling LED:', err);
        alert('Failed to toggle LED. Please try again.');
      } finally {
        hideSpinner(); // Hide loading animation
      }
    }

    function showSpinner() {
      document.getElementById('loading').style.display = 'flex'; // Show the loading animation
    }

    function hideSpinner() {
      document.getElementById('loading').style.display = 'none'; // Hide the loading animation
    }

    // Fetch the LED state when the page loads
    window.onload = fetchLEDState;
  </script>
</body>
</html>
)rawliteral";

// Handle requests for the root page
void handleRoot() {
  String html = FPSTR(webpage); // Read HTML content from program memory
  html.replace("%LED_STATE%", ledState ? "ON" : "OFF");
  server.send(200, "text/html", html);
}

// Handle requests to turn the LED ON
void handleLEDOn() {
  Serial.println("LED ON requested");
  digitalWrite(ledPin, LOW);
  ledState = true;
  server.send(200, "text/plain", "ON");
}

// Handle requests to turn the LED OFF
void handleLEDOff() {
  Serial.println("LED OFF requested");
  digitalWrite(ledPin, HIGH);
  ledState = false;
  server.send(200, "text/plain", "OFF");
}

void setup() {
  // Start Serial communication for debugging
  Serial.begin(115200);

  // Configure LED pin as output
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Turn LED off initially

  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  // Print connection details
  Serial.println("\nConnected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); // Print the ESP8266's IP address

  // Define URL handlers
  server.on("/", handleRoot);      // Serve the HTML page
  server.on("/on", handleLEDOn);  // Handle LED ON request
  server.on("/off", handleLEDOff);// Handle LED OFF request

  // Start the web server
  server.begin();
  Serial.println("Web server started!");
}

void loop() {
  // Measure and debug request handling time
  unsigned long start = millis();
  server.handleClient();
  unsigned long duration = millis() - start;
  if (duration > 50) { // Log slow request handling
    Serial.print("Slow request handling: ");
    Serial.print(duration);
    Serial.println(" ms");
  }
}
