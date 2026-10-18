import { useEffect, useState } from "react";
import mqttClient from "./mqttClient";
import "./App.css";

const TOPICS = [
  "aquasync/turbidity",
  "aquasync/ph",
  "aquasync/temperature",
  "aquasync/sensors",
];

function parsePayload(message) {
  const text = message.toString().trim();
  try {
    return JSON.parse(text);
  } catch {
    const value = Number(text);
    return Number.isFinite(value) ? value : null;
  }
}

function numericValue(data, keys) {
  if (typeof data === "number") return data;
  if (!data || typeof data !== "object") return null;
  for (const key of keys) {
    const value = data[key];
    if (typeof value === "number" && Number.isFinite(value)) return value;
    if (typeof value === "string" && value.trim() !== "" && Number.isFinite(Number(value))) {
      return Number(value);
    }
  }
  return null;
}

export default function AquaSyncPage() {
  const [mqttConnected, setMqttConnected] = useState(Boolean(mqttClient.connected));
  const [sensorData, setSensorData] = useState({ turbidity: null, ph: null, temperature: null });
  const [lastUpdate, setLastUpdate] = useState(null);
  const [connectionMessage, setConnectionMessage] = useState(
    mqttClient.connected ? "Connected to MQTT broker" : "Connecting to MQTT broker…"
  );

  useEffect(() => {
    const subscribe = () => {
      if (!mqttClient.connected) return;
      setMqttConnected(true);
      setConnectionMessage("Connected to MQTT broker");
      mqttClient.subscribe(TOPICS, { qos: 0 }, (error) => {
        if (error) {
          console.error("AquaSync subscription failed:", error);
          setConnectionMessage("Unable to subscribe to sensor topics");
        }
      });
    };
    const onConnect = () => subscribe();
    const onDisconnect = () => {
      setMqttConnected(false);
      setConnectionMessage("MQTT disconnected");
    };
    const onOffline = () => {
      setMqttConnected(false);
      setConnectionMessage("MQTT offline — waiting to reconnect");
    };

    mqttClient.on("connect", onConnect);
    mqttClient.on("disconnect", onDisconnect);
    mqttClient.on("close", onDisconnect);
    mqttClient.on("offline", onOffline);
    if (mqttClient.connected) subscribe();

    return () => {
      mqttClient.off("connect", onConnect);
      mqttClient.off("disconnect", onDisconnect);
      mqttClient.off("close", onDisconnect);
      mqttClient.off("offline", onOffline);
      if (mqttClient.connected) mqttClient.unsubscribe(TOPICS);
    };
  }, []);

  useEffect(() => {
    const handleMessage = (topic, message) => {
      if (!TOPICS.includes(topic)) return;
      const data = parsePayload(message);
      if (data === null) return;

      setSensorData((previous) => {
        const next = { ...previous };
        if (topic === "aquasync/turbidity") {
          const rawADC = numericValue(data, ["rawADC", "rawAdc", "adc"]);
          const status = typeof data === "object" ? data.status : null;
          if (rawADC !== null || status) {
            next.turbidity = {
              ...(previous.turbidity || {}),
              ...(rawADC !== null ? { rawADC } : {}),
              ...(typeof status === "string" ? { status: status.toUpperCase() } : {}),
              adcVoltage: numericValue(data, ["adcVoltage"]),
              sensorVoltage: numericValue(data, ["sensorVoltage"]),
            };
          } else if (typeof data === "number") {
            next.turbidity = { ...(previous.turbidity || {}), rawADC: data };
          }
        } else if (topic === "aquasync/ph") {
          const value = numericValue(data, ["ph", "pH", "value"]);
          if (value !== null) next.ph = value;
        } else if (topic === "aquasync/temperature") {
          const value = numericValue(data, ["temperature", "temp", "waterTemperature", "value"]);
          if (value !== null) next.temperature = value;
        } else if (topic === "aquasync/sensors" && typeof data === "object") {
          const ph = numericValue(data, ["ph", "pH"]);
          const temperature = numericValue(data, ["temperature", "temp", "waterTemperature"]);
          const rawADC = numericValue(data, ["rawADC", "rawAdc", "turbidityADC"]);
          if (ph !== null) next.ph = ph;
          if (temperature !== null) next.temperature = temperature;
          if (rawADC !== null || typeof data.turbidityStatus === "string" || typeof data.status === "string") {
            next.turbidity = {
              ...(previous.turbidity || {}),
              ...(rawADC !== null ? { rawADC } : {}),
              ...((data.turbidityStatus || data.status) ? { status: String(data.turbidityStatus || data.status).toUpperCase() } : {}),
            };
          }
        }
        return next;
      });
      setLastUpdate(Date.now());
    };

    mqttClient.on("message", handleMessage);
    return () => mqttClient.off("message", handleMessage);
  }, []);

  const turbidityStatus = sensorData.turbidity?.status || "WAITING FOR DATA";
  const turbidityClass = !sensorData.turbidity
    ? "aquasync-muted"
    : turbidityStatus === "CLEAR"
    ? "aquasync-good"
    : turbidityStatus.includes("CLOUDY")
    ? "aquasync-warn"
    : "aquasync-muted";

  return (
    <div className="dashboard aquasync-page">
      <aside className="sidebar">
        <div className="brand">
          <div className="brand-icon">🌊</div>
          <div><h2>AquaSync</h2><span>Smart Pond Monitoring</span></div>
        </div>
        <nav>
          <a href="/">🤖 AgriBot Dashboard</a>
          <a href="/?page=aquasync" className="active">🌊 AquaSync</a>
        </nav>
        <div className="sidebar-bottom">
          <div className="system-small">
            <span className={`status-dot ${mqttConnected ? "" : "offline"}`} />
            {mqttConnected ? "MQTT Connected" : "MQTT Offline"}
          </div>
          <p>Smart water monitoring<br />FloraSync ecosystem.</p>
        </div>
      </aside>

      <main className="main aquasync-main">
        <header className="topbar aquasync-topbar">
          <div>
            <p className="eyebrow">FLORASYNC · WATER QUALITY</p>
            <h1>AquaSync</h1>
            <p className="aquasync-subtitle">A quick overview of your pond's live sensor readings.</p>
          </div>
          <div className="connection">
            <span className={`status-dot ${mqttConnected ? "" : "offline"}`} />
            <div><strong>{mqttConnected ? "Live connection" : "Offline"}</strong><small>{connectionMessage}</small></div>
          </div>
        </header>

        <section className="aquasync-overview">
          <div className="aquasync-section-heading">
            <div><span className="section-label">LIVE OVERVIEW</span><h2>Water parameters</h2></div>
            <span className={`aquasync-live-pill ${mqttConnected ? "is-live" : ""}`}><span />{mqttConnected ? "MQTT READY" : "RECONNECTING"}</span>
          </div>

          <div className="aquasync-metric-grid">
            <article className="aquasync-metric-card">
              <div className="aquasync-metric-top"><span className="aquasync-metric-icon ph-icon">pH</span><span className="aquasync-metric-label">ACIDITY LEVEL</span></div>
              <div className="aquasync-metric-value">{sensorData.ph !== null ? sensorData.ph.toFixed(2) : "--"}</div>
              <div className="aquasync-metric-unit">pH units</div>
              <div className="aquasync-metric-foot">{sensorData.ph !== null ? "Latest sensor reading" : "Waiting for pH sensor data"}</div>
            </article>

            <article className="aquasync-metric-card">
              <div className="aquasync-metric-top"><span className="aquasync-metric-icon temp-icon">°C</span><span className="aquasync-metric-label">WATER TEMPERATURE</span></div>
              <div className="aquasync-metric-value">{sensorData.temperature !== null ? sensorData.temperature.toFixed(1) : "--"}<span className="aquasync-value-suffix">{sensorData.temperature !== null ? "°C" : ""}</span></div>
              <div className="aquasync-metric-unit">Water temperature</div>
              <div className="aquasync-metric-foot">{sensorData.temperature !== null ? "Latest sensor reading" : "Waiting for temperature data"}</div>
            </article>

            <article className="aquasync-metric-card">
              <div className="aquasync-metric-top"><span className="aquasync-metric-icon turbidity-icon">〰</span><span className="aquasync-metric-label">WATER CLARITY</span></div>
              <div className={`aquasync-metric-status ${turbidityClass}`}>{turbidityStatus}</div>
              <div className="aquasync-metric-value aquasync-adc-value">{sensorData.turbidity?.rawADC ?? "--"}<span className="aquasync-value-suffix">{sensorData.turbidity?.rawADC !== undefined ? " ADC" : ""}</span></div>
              <div className="aquasync-metric-foot">Turbidity sensor raw reading</div>
            </article>
          </div>

          <div className="aquasync-bottom-row">
            <div className="aquasync-info-card">
              <div className="aquasync-info-icon">⟳</div>
              <div><span className="section-label">LAST UPDATE</span><strong>{lastUpdate ? new Date(lastUpdate).toLocaleTimeString() : "Waiting for readings"}</strong><p>Updates when a sensor message arrives over MQTT.</p></div>
            </div>
            <div className="aquasync-info-card">
              <div className="aquasync-info-icon">ⓘ</div>
              <div><span className="section-label">SENSOR NOTE</span><strong>Readings depend on calibration</strong><p>pH and temperature need working sensors. Turbidity status uses experimental ADC thresholds, not calibrated NTU.</p></div>
            </div>
          </div>
        </section>

        <footer><span>FloraSync · AquaSync © 2026</span><span>IoT Water Monitoring</span></footer>
      </main>
    </div>
  );
}
