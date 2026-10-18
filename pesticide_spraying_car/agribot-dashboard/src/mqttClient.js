import mqtt from "mqtt";

const client = mqtt.connect(
  "wss://aa595fb1583d4754a76c3f3ff4fbb9e3.s1.eu.hivemq.cloud:8884/mqtt",
  {
    username: "florasync",
    password: "florasync@123",

    reconnectPeriod: 1000,
    connectTimeout: 10000,
    keepalive: 30,
  }
);

client.on("connect", () => {
  console.log("🟢 MQTT Connected!");
});

client.on("reconnect", () => {
  console.log("🟡 MQTT Reconnecting...");
});

client.on("offline", () => {
  console.log("🔴 MQTT Offline");
});

client.on("close", () => {
  console.log("🔴 MQTT Disconnected");
});

client.on("error", (error) => {
  console.error("MQTT Error:", error);
});

export default client;