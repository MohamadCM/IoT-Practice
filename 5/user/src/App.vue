<template>
  <v-app>
    <v-app-bar
        app
        color="primary"
        dark
    >
      <div class="d-flex align-center">
        IoT platform homework
      </div>

      <v-spacer></v-spacer>
      <span class="mr-2">Latest Release</span>
      <v-icon>mdi-open-in-new</v-icon>
    </v-app-bar>

    <v-main>
      <v-content v-if="!loggedIn" class="ma-5">
        <v-row no-gutters class="align-center justify-center">
          <v-col
              cols="12"
              md="4"
              class="ma-1"
          >
            <v-text-field
                v-model="userAccessName"
                label="Username"
                append-icon="mdi-domain"
            ></v-text-field>
          </v-col>
          <v-col
              cols="12"
              md="4"
              class="ma-1"
          >
            <v-btn color="primary" value="ورود" @click="login" :disabled="!userAccessName">
              ورود
            </v-btn>
          </v-col>
        </v-row>
      </v-content>
      <v-content v-else>

        <v-row class="ma-10">
          <v-text-field disabled label="مقدار سنسور دما" v-model="temp"/>
          <v-btn color="primary" @click="getTemp">
            به روز رسانی
          </v-btn>
        </v-row>
        <v-row class="ma-10">
          <v-text-field disabled label="مقدار سنسور نور" v-model="light"/>
          <v-btn color="primary" @click="getLight">
            به روز رسانی
          </v-btn>
        </v-row>

        <v-row class="ma-10">
          <v-text-field label="آستانه دما" v-model="tempThreshold"/>
          <v-btn color="primary" @click="setTemp">
            ثبت آستانه دما
          </v-btn>
          <v-spacer/>
          <v-text-field label="آستانه نور" v-model="lightThreshold"/>
          <v-btn color="primary" @click="setLight">
            ثبت آستانه دما نور
          </v-btn>
        </v-row>

        <v-row class="ma-10">
          <v-text-field label="ID" v-model="sensor.ID" class="ma-2"/>
          <v-text-field label="address" v-model="sensor.address" class="ma-2"/>
          <v-text-field label="entry-code" v-model="sensor.entry_code" class="ma-2"/>
          <v-btn color="primary" @click="registerSensor">
            ثبت سنسور/عملگر
          </v-btn>
        </v-row>

        <v-row class="ma-10" style="direction: rtl">
          <v-checkbox
              v-model="fan"
              label="وضعیت فن"
          ></v-checkbox>
          <v-btn color="primary" @click="setFan">
            ثبت وضعیت فن
          </v-btn>
          <v-spacer />
          <v-checkbox
              v-model="lamp"
              label="وضعیت لامپ"
          ></v-checkbox>
          <v-btn color="primary" @click="setLamp">
            ثبت وضعیت لامپ
          </v-btn>
        </v-row>
        <v-btn color="primary" @click="dialog = !dialog" block>
          نمایش نمودار‌ها
        </v-btn>
        <v-dialog
            v-model="dialog"
            style="z-index: 100; background: white"
            transition="dialog-bottom-transition"
        >
          <diagrams v-if="dialog" style="z-index: 100; background: white"/>
        </v-dialog>
      </v-content>
    </v-main>
  </v-app>
</template>

<script>

import axios from "axios";
import Diagrams from "./Diagrams"
export default {
  name: "App",
  components: {
    Diagrams
  },
  data: () => ({
    mainServer: "http://localhost:8000",
    localServerAddress: "http://localhost:5000",
    loggedIn: false,
    userAccessName: undefined,
    temp: undefined,
    light: undefined,
    tempThreshold: undefined,
    lightThreshold: undefined,
    sensor: {ID: undefined, address: undefined, entry_code: undefined},
    fan: false,
    lamp: true,
    dialog: false
  }),
  methods: {
    async login() {
      try {
        const response = await axios.post(this.mainServer + "/user/auth", {}, {
          params: {access_name: this.userAccessName}
        });
        const token = response.data.token;
        localStorage.setItem("platform-token", token);
        this.loggedIn = true;
      } catch (e) {
        console.error("Error logging in");
      }
    },
    async getTemp() {
      try {
        const response = await axios.get(this.localServerAddress + "/sensor", {
          params: {ID: "sensor-0"}
        });
        this.temp = response.data.data.SENSOR_VALUE;
      } catch (e) {
        console.error("Error getting data");
      }
    },
    async getLight() {
      try {
        const response = await axios.get(this.localServerAddress + "/sensor", {
          params: {ID: "sensor-1"}
        });
        this.temp = response.data.data.SENSOR_VALUE;
      } catch (e) {
        console.error("Error getting data");
      }
    },
    async setTemp() {
      try {
        await axios.post(this.localServerAddress + "/sensor/threshold", {}, {
          params: {temp: this.tempThreshold}
        });
      } catch (e) {
        console.error("Error setting data");
      }
    },
    async setLight() {
      try {
        await axios.post(this.localServerAddress + "/sensor/threshold", {}, {
          params: {light: this.lightThreshold}
        });
      } catch (e) {
        console.error("Error setting data");
      }
    },
    async registerSensor() {
      try {
        const response = await axios.post(this.mainServer + "/thing/register", this.sensor, {
          params: this.sensor,
          headers: {
            authorization: `Bearer ${localStorage.getItem("platform-token")}`
          }
        });
        console.log(response);
      } catch (e) {
        console.log(e);
        console.error("Error registering thing");
      }
    },
    async setLamp() {
      try {
        const response = await axios.post(this.localServerAddress + "/actuator", {}, {
          params: {ID: "actuator-1", request: this.lamp.toString()},
          headers: {
            authorization: `Bearer ${localStorage.getItem("platform-token")}`
          }
        });
        console.log(response);
      } catch (e) {
        console.log(e);
      }
    },
    async setFan() {
      try {
        const response = await axios.post(this.localServerAddress + "/actuator", {}, {
          params: {ID: "actuator-0", request: this.lamp.toString()},
          headers: {
            authorization: `Bearer ${localStorage.getItem("platform-token")}`
          }
        });
        if(response.status % 100 === 2){
          console.log(response.data.data);
        }
      } catch (e) {
        console.log(e);
      }
    }
  }
};
</script>
