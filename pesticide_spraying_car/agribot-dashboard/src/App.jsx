import { useEffect, useRef, useState } from "react";

import mqttClient from "./mqttClient";
import AquaSyncPage from "./AquaSync";

import "./App.css";



// =====================================================

// ESP32-CAM CONFIGURATION

// =====================================================


const CAMERA_IP = "192.168.1.12";
const CAMERA_URL = `http://${CAMERA_IP}:81/stream`;

// =====================================================

// MQTT TOPICS

// =====================================================



const HEARTBEAT_TOPIC = "agribot/status/online";

const STATUS_TOPIC = "agribot/status";

const MOVEMENT_TOPIC = "agribot/movement";

const SPRAYER_TOPIC = "agribot/sprayer";
const MOTOR1_TOPIC = "agribot/motor1";
const MOTOR2_TOPIC = "agribot/motor2";



// =====================================================

// APP

// =====================================================



function AgribotDashboard() {



  // =====================================================

  // MQTT

  // =====================================================



  const [mqttConnected, setMqttConnected] = useState(

    mqttClient.connected

  );



  // =====================================================

  // ROBOT

  // =====================================================



  const [robotOnline, setRobotOnline] = useState(false);



  const [robotStatus, setRobotStatus] =

    useState("OFFLINE");



  const [lastHeartbeat, setLastHeartbeat] =

    useState(null);

  const lastHeartbeatRef = useRef(null);

  useEffect(() => {
    lastHeartbeatRef.current = lastHeartbeat;
  }, [lastHeartbeat]);



  // =====================================================

  // SPRAYER

  // =====================================================



  const [spraying, setSpraying] =

    useState(false);

  const [motor1On, setMotor1On] = useState(false);
  const [motor2On, setMotor2On] = useState(false);



  // =====================================================

  // SENSOR DATA

  // =====================================================



  const [battery, setBattery] =

    useState(null);



  const [tankLevel, setTankLevel] =

    useState(null);



  // =====================================================

  // CAMERA

  // =====================================================



  const [cameraOnline, setCameraOnline] =

    useState(true);




  // =====================================================

  // MQTT CONNECTION

  // =====================================================



  useEffect(() => {



    const subscribeToRobot = () => {



      if (!mqttClient.connected) {

        return;

      }



      console.log(

        "📡 Subscribing to Agribot topics..."

      );



      mqttClient.subscribe(

        HEARTBEAT_TOPIC,

        { qos: 1 },

        (error) => {



          if (error) {



            console.error(

              "❌ Heartbeat subscription failed:",

              error

            );



          } else {



            console.log(

              "✅ Subscribed:",

              HEARTBEAT_TOPIC

            );



          }



        }

      );



      mqttClient.subscribe(

        STATUS_TOPIC,

        { qos: 1 },

        (error) => {



          if (error) {



            console.error(

              "❌ Status subscription failed:",

              error

            );



          } else {



            console.log(

              "✅ Subscribed:",

              STATUS_TOPIC

            );



          }



        }

      );



    };



    // ===================================================

    // CONNECT

    // ===================================================



    const handleConnect = () => {



      console.log(

        "🟢 DASHBOARD MQTT CONNECTED"

      );



      setMqttConnected(true);



      subscribeToRobot();



    };



    // ===================================================

    // DISCONNECT

    // ===================================================



    const handleDisconnect = () => {



      console.log(

        "🔴 DASHBOARD MQTT DISCONNECTED"

      );



      setMqttConnected(false);



      setRobotOnline(false);



      setRobotStatus("OFFLINE");



      setSpraying(false);



    };



    // ===================================================

    // EVENTS

    // ===================================================



    mqttClient.on(

      "connect",

      handleConnect

    );



    mqttClient.on(

      "disconnect",

      handleDisconnect

    );



    mqttClient.on(

      "close",

      handleDisconnect

    );



    // ===================================================

    // ALREADY CONNECTED

    // ===================================================



    if (mqttClient.connected) {



      setMqttConnected(true);



      subscribeToRobot();



    }



    // ===================================================

    // CLEANUP

    // ===================================================



    return () => {



      mqttClient.off(

        "connect",

        handleConnect

      );



      mqttClient.off(

        "disconnect",

        handleDisconnect

      );



      mqttClient.off(

        "close",

        handleDisconnect

      );



    };



  }, []);



  // =====================================================

  // MQTT MESSAGE HANDLER

  // =====================================================



  useEffect(() => {



    const handleMessage = (

      topic,

      message

    ) => {



      const payload =

        message.toString().trim();



      console.log(

        "📨 MQTT:",

        topic,

        payload

      );



      // =================================================

      // AQUASYNC TURBIDITY DATA
      // =================================================
      // HEARTBEAT

      // =================================================



      if (

        topic === HEARTBEAT_TOPIC

      ) {



        if (

          payload === "ONLINE"

        ) {



          setRobotOnline(true);



          setLastHeartbeat(

            Date.now()

          );



          setRobotStatus(

            (current) => {



              if (

                current === "OFFLINE"

              ) {



                return "READY";



              }



              return current;



            }

          );



        }



      }



      // =================================================

      // ROBOT STATUS

      // =================================================



      if (

        topic === STATUS_TOPIC

      ) {



        try {



          const data =

            JSON.parse(payload);



          console.log(

            "🤖 ROBOT STATUS:",

            data

          );



          // ---------------------------------------------

          // ONLINE

          // ---------------------------------------------



          if (

            data.online === true

          ) {



            setRobotOnline(true);



            setLastHeartbeat(

              Date.now()

            );



          }



          // ---------------------------------------------

          // MOVEMENT

          // ---------------------------------------------



          if (

            typeof data.movement === "string"

          ) {



            setRobotStatus(

              data.movement

            );



          }



          // ---------------------------------------------

          // SPRAYER

          // ---------------------------------------------



          if (

            typeof data.sprayer === "boolean"

          ) {



            setSpraying(

              data.sprayer

            );



          }



          // ---------------------------------------------

          // BATTERY

          // ---------------------------------------------



          if (

            typeof data.battery === "number"

          ) {



            setBattery(

              data.battery

            );



          }



          // ---------------------------------------------

          // TANK

          // ---------------------------------------------



          if (

            typeof data.tank === "number"

          ) {



            setTankLevel(

              data.tank

            );



          }



        } catch (error) {



          console.error(

            "❌ Invalid robot JSON:",

            error

          );



        }



      }



    };



    mqttClient.on(

      "message",

      handleMessage

    );



    return () => {



      mqttClient.off(

        "message",

        handleMessage

      );



    };



  }, []);



  // =====================================================

  // ROBOT HEARTBEAT TIMEOUT

  // =====================================================

  useEffect(() => {
    const timer = setInterval(() => {
      const heartbeat = lastHeartbeatRef.current;

      if (!heartbeat || Date.now() - heartbeat > 6000) {
        setRobotOnline(false);
        setRobotStatus("OFFLINE");
        setSpraying(false);
      }
    }, 1000);

    return () => clearInterval(timer);
  }, []);

  // =====================================================

  // =====================================================

  // CAMERA ERROR

  // =====================================================



  const handleCameraError = () => {



    console.log(

      "🔴 ESP32-CAM stream connection failed"

    );



    setCameraOnline(false);



  };



  // =====================================================

  // CAMERA LOADED

  // =====================================================



  const handleCameraLoad = () => {



    console.log(

      "🟢 ESP32-CAM stream active"

    );



    setCameraOnline(true);



  };



  // =====================================================

  // MOVEMENT

  // =====================================================



  const sendCommand = (

    command

  ) => {



    if (

      !mqttClient.connected

    ) {



      console.log(

        "❌ MQTT disconnected"

      );



      setMqttConnected(false);



      return;



    }



    if (

      !robotOnline

    ) {



      console.log(

        "❌ Robot offline"

      );



      return;



    }



    console.log(

      "🚜 Sending:",

      command

    );



    mqttClient.publish(

      MOVEMENT_TOPIC,

      command,

      {

        qos: 1,

        retain: false

      },

      (error) => {



        if (error) {



          console.error(

            "❌ Command failed:",

            error

          );



          return;



        }



        console.log(

          "✅ Command sent:",

          command

        );



      }

    );



    switch (command) {



      case "FORWARD":



        setRobotStatus(

          "MOVING FORWARD"

        );



        break;



      case "BACKWARD":



        setRobotStatus(

          "MOVING BACKWARD"

        );



        break;



      case "LEFT":



        setRobotStatus(

          "TURNING LEFT"

        );



        break;



      case "RIGHT":



        setRobotStatus(

          "TURNING RIGHT"

        );



        break;



      case "STOP":



        setRobotStatus(

          "STOPPED"

        );



        break;



      default:

        break;



    }



  };



  // =====================================================

  // EMERGENCY STOP

  // =====================================================



  const stopRobot = () => {



    if (

      !mqttClient.connected

    ) {



      console.log(

        "❌ MQTT disconnected"

      );



      return;



    }



    if (

      !robotOnline

    ) {



      console.log(

        "❌ Robot offline"

      );



      return;



    }



    mqttClient.publish(

      MOVEMENT_TOPIC,

      "STOP",

      {

        qos: 1,

        retain: false

      }

    );



    mqttClient.publish(

      SPRAYER_TOPIC,

      "SPRAY_OFF",

      {

        qos: 1,

        retain: false

      }

    );

    mqttClient.publish(MOTOR1_TOPIC, "OFF", { qos: 1, retain: false });
    mqttClient.publish(MOTOR2_TOPIC, "OFF", { qos: 1, retain: false });



    setRobotStatus(

      "STOPPED"

    );



    setSpraying(

      false

    );



    console.log(

      "🛑 EMERGENCY STOP"

    );



  };



  // =====================================================

  // SPRAYER

  // =====================================================



  const toggleSprayer = () => {



    if (

      !mqttClient.connected

    ) {



      console.log(

        "❌ MQTT disconnected"

      );



      return;



    }



    if (

      !robotOnline

    ) {



      console.log(

        "❌ Robot offline"

      );



      return;



    }



    const command =

      spraying

        ? "SPRAY_OFF"

        : "SPRAY_ON";



    mqttClient.publish(

      SPRAYER_TOPIC,

      command,

      {

        qos: 1,

        retain: false

      },

      (error) => {



        if (error) {



          console.error(

            "❌ Sprayer error:",

            error

          );



          return;



        }



        setSpraying(

          !spraying

        );



      }

    );



  };



  // =====================================================

  // PUMP / MOTOR RELAYS
  // =====================================================

  const toggleMotor = (motorNumber) => {
    if (!mqttClient.connected || !robotOnline) {
      console.log("❌ MQTT disconnected or robot offline");
      return;
    }

    const isMotorOn = motorNumber === 1 ? motor1On : motor2On;
    const topic = motorNumber === 1 ? MOTOR1_TOPIC : MOTOR2_TOPIC;
    const command = isMotorOn ? "OFF" : "ON";

    mqttClient.publish(topic, command, { qos: 1, retain: false }, (error) => {
      if (error) {
        console.error(`❌ Motor ${motorNumber} command failed:`, error);
        return;
      }
      if (motorNumber === 1) setMotor1On(command === "ON");
      else setMotor2On(command === "ON");
      console.log(`✅ Motor ${motorNumber}: ${command}`);
    });
  };

  const stopMotors = () => {
    if (!mqttClient.connected || !robotOnline) {
      console.log("❌ Cannot stop pumps: MQTT disconnected or robot offline");
      return;
    }

    mqttClient.publish(
      MOTOR1_TOPIC,
      "OFF",
      { qos: 1, retain: false },
      (error) => {
        if (error) {
          console.error("❌ Failed to stop Pump 1:", error);
          return;
        }
        setMotor1On(false);
        console.log("🛑 Pump 1 OFF command published");
      }
    );

    mqttClient.publish(
      MOTOR2_TOPIC,
      "OFF",
      { qos: 1, retain: false },
      (error) => {
        if (error) {
          console.error("❌ Failed to stop Pump 2:", error);
          return;
        }
        setMotor2On(false);
        console.log("🛑 Pump 2 OFF command published");
      }
    );
  };

  // =====================================================
  // // ROBOT STATUS COLOR

  // =====================================================



  const getRobotStatusClass = () => {



    if (

      !robotOnline

    ) {



      return "danger-text";



    }



    if (

      robotStatus === "READY" ||

      robotStatus === "STOPPED"

    ) {



      return "ready-text";



    }



    return "active-text";



  };



  // =====================================================

  // RENDER

  // =====================================================



  return (



    <div className="dashboard">



      {/* =================================================

          SIDEBAR

      ================================================= */}



      <aside className="sidebar">



        <div className="brand">



          <div className="brand-icon">

            🌱

          </div>



          <div>



            <h2>

              FloraSync

            </h2>



            <span>

              AgriBot Control

            </span>



          </div>



        </div>



        <nav>



          <a className="active">

            🤖 Dashboard

          </a>



          <a>

            📷 Camera

          </a>



          <a>

            🌱 Crop Monitor

          </a>



          <a>

            💧 Sprayer

          </a>



          <a>

            ⚙️ Settings

          </a>

          <a href="/?page=aquasync">🌊 AquaSync</a>



        </nav>



        <div className="sidebar-bottom">



          <div className="system-small">



            <span

              className={`status-dot ${

                robotOnline

                  ? ""

                  : "offline"

              }`}

            />



            {robotOnline

              ? "Robot Online"

              : "Robot Offline"}



          </div>



          <p>

            Smart Agriculture

            <br />

            Brighter Tomorrows.

          </p>



        </div>



      </aside>



      {/* =================================================

          MAIN

      ================================================= */}



      <main className="main">



        {/* =================================================

            HEADER

        ================================================= */}



        <header className="topbar">



          <div>



            <p className="eyebrow">

              FARM ROBOTICS

            </p>



            <h1>

              AgriBot Dashboard

            </h1>



          </div>



          <div className="connection">



            <span

              className={`status-dot ${

                mqttConnected

                  ? ""

                  : "offline"

              }`}

            />



            <div>



              <strong>



                {mqttConnected

                  ? "MQTT Connected"

                  : "MQTT Offline"}



              </strong>



              <small>



                {mqttConnected

                  ? "Dashboard communication active"

                  : "Dashboard communication lost"}



              </small>



            </div>



          </div>



        </header>



        {/* =================================================

            STATUS CARDS

        ================================================= */}



        <section className="stats">



          {/* ROBOT */}



          <div className="stat-card">



            <div className="stat-icon">

              🤖

            </div>



            <div>



              <span>

                Robot Status

              </span>



              <strong

                className={

                  getRobotStatusClass()

                }

              >



                {robotOnline

                  ? robotStatus

                  : "OFFLINE"}



              </strong>



            </div>



          </div>



          {/* BATTERY */}



          <div className="stat-card">



            <div className="stat-icon">

              🔋

            </div>



            <div>



              <span>

                Battery

              </span>



              <strong>



                {battery !== null

                  ? `${battery}%`

                  : "NOT AVAILABLE"}



              </strong>



            </div>



          </div>



          {/* SPRAYER */}



          <div className="stat-card">



            <div className="stat-icon">

              💧

            </div>



            <div>



              <span>

                Sprayer

              </span>



              <strong

                className={

                  spraying

                    ? "active-text"

                    : ""

                }

              >



                {!robotOnline

                  ? "OFFLINE"

                  : spraying

                  ? "SPRAYING"

                  : "OFF"}



              </strong>



            </div>



          </div>



          {/* CAMERA */}



          <div className="stat-card">



            <div className="stat-icon">

              📷

            </div>



            <div>



              <span>

                Camera

              </span>



              <strong

                className={

                  cameraOnline

                    ? "ready-text"

                    : "danger-text"

                }

              >



                {cameraOnline

                  ? "LIVE"

                  : "OFFLINE"}



              </strong>



            </div>



          </div>



        </section>



        {/* =================================================

            MAIN GRID

        ================================================= */}



        <section className="content-grid">



          {/* =================================================

              MOVEMENT

          ================================================= */}



          <div className="panel movement-panel">



            <div className="panel-header">



              <div>



                <span className="section-label">

                  ROBOT CONTROL

                </span>



                <h2>

                  Movement

                </h2>



              </div>



              <span className="control-mode">

                MANUAL

              </span>



            </div>



            <div className="controller">



              <button

                className="direction up"

                onClick={() =>

                  sendCommand("FORWARD")

                }

                disabled={!robotOnline}

              >



                ↑



                <span>

                  Forward

                </span>



              </button>



              <button

                className="direction left"

                onClick={() =>

                  sendCommand("LEFT")

                }

                disabled={!robotOnline}

              >



                ←



                <span>

                  Left

                </span>



              </button>



              <button

                className="stop"

                onClick={stopRobot}

                disabled={!robotOnline}

              >



                STOP



              </button>



              <button

                className="direction right"

                onClick={() =>

                  sendCommand("RIGHT")

                }

                disabled={!robotOnline}

              >



                →



                <span>

                  Right

                </span>



              </button>



              <button

                className="direction down"

                onClick={() =>

                  sendCommand("BACKWARD")

                }

                disabled={!robotOnline}

              >



                ↓



                <span>

                  Backward

                </span>



              </button>



            </div>



            <p className="control-hint">



              {robotOnline

                ? "Robot is ready for manual control."

                : "Turn on the Agribot to enable controls."}



            </p>



          </div>



          {/* =================================================

              CAMERA

          ================================================= */}



          <div className="panel camera-panel">



            <div className="panel-header">



              <div>



                <span className="section-label">

                  VISION SYSTEM

                </span>



                <h2>

                  Robot Camera

                </h2>



              </div>



              <span className="live-badge">



                <span className="camera-live-dot"></span>



                LIVE



              </span>



            </div>



            {/* CAMERA STREAM */}



            <div className="camera-feed-container">



              <img

                key={CAMERA_URL}

                src={CAMERA_URL}

                alt="FloraSync AgriBot Live Camera"

                className="camera-feed"

                onLoad={handleCameraLoad}

                onError={handleCameraError}

              />



              <div className="camera-live-overlay">



                <span className="camera-live-dot"></span>



                LIVE



              </div>



              <div className="camera-name-overlay">



                📷 ESP32-CAM



              </div>



            </div>



            {/* CAMERA INFORMATION */}



            <div className="camera-info">



              <span>

                📷 ESP32-CAM

              </span>



              <span>

                {CAMERA_IP}

              </span>



            </div>



          </div>



        </section>



        {/* =================================================
            PUMP / MOTOR CONTROL
        ================================================= */}

        <section className="panel sprayer-panel">
          <div className="sprayer-info">
            <div className="sprayer-icon">🚿</div>
            <div>
              <span className="section-label">PUMP CONTROL</span>
              <h2>Water Pump Motors</h2>
              <p>Control the two diaphragm pumps through the relay module.</p>
            </div>
          </div>
          <div style={{ display: "flex", gap: "12px", flexWrap: "wrap", alignItems: "center" }}>
            <button className={`spray-button ${motor1On ? "spraying" : ""}`} onClick={() => toggleMotor(1)} disabled={!robotOnline}>
              <span>{motor1On ? "■" : "▶"}</span> {motor1On ? "STOP MOTOR 1" : "START MOTOR 1"}
            </button>
            <button className={`spray-button ${motor2On ? "spraying" : ""}`} onClick={() => toggleMotor(2)} disabled={!robotOnline}>
              <span>{motor2On ? "■" : "▶"}</span> {motor2On ? "STOP MOTOR 2" : "START MOTOR 2"}
            </button>
            <button className="stop" onClick={stopMotors} disabled={!robotOnline}>STOP BOTH</button>
          </div>
        </section>



        {/* =================================================

            FOOTER

        ================================================= */}



        <footer>



          <span>

            FloraSync © 2026

          </span>



          <span>

            Smart Agriculture • IoT • AI

          </span>



        </footer>



      </main>



    </div>



  );



}



function App() {
  const isAquaSyncPage =
    new URLSearchParams(window.location.search).get("page") === "aquasync" ||
    window.location.pathname.replace(/\/$/, "") === "/aquasync";

  return isAquaSyncPage ? <AquaSyncPage /> : <AgribotDashboard />;
}

export default App;